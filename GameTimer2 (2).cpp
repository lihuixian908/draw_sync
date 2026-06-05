#include "GameTimer.h"

GuessInputDialog::GuessInputDialog(int wordLength, QWidget *parent)
    : QDialog(parent), m_wordLength(wordLength)
{
    setWindowTitle("猜单词");
    setModal(false);
    setFixedSize(400, 200);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_timeLabel = new QLabel("剩余时间: -- 秒", this);
    mainLayout->addWidget(m_timeLabel);

    m_displayLabel = new QLabel(this);
    QFont displayFont("Monospace", 16, QFont::Bold);
    m_displayLabel->setFont(displayFont);
    m_displayLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_displayLabel);

    m_hiddenInput = new QLineEdit(this);
    // 移除字数限制，允许输入任意长度的猜测
    // m_hiddenInput->setMaxLength(m_wordLength);
    m_hiddenInput->setVisible(false);

    m_submitButton = new QPushButton("提交答案", this);
    m_submitButton->setEnabled(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_submitButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    connect(m_hiddenInput, &QLineEdit::textChanged, this, &GuessInputDialog::onTextChanged);
    connect(m_submitButton, &QPushButton::clicked, this, &GuessInputDialog::onSubmitClicked);
    
    // 连接回车键提交
    connect(m_hiddenInput, &QLineEdit::returnPressed, this, &GuessInputDialog::onSubmitClicked);

    updateDisplay();

    m_hiddenInput->setFocus();
}

void GuessInputDialog::setRemainingTime(int seconds)
{
    m_timeLabel->setText(QString("剩余时间: %1 秒").arg(seconds));
}

QString GuessInputDialog::getGuess() const
{
    return m_hiddenInput->text().trimmed();
}

void GuessInputDialog::clearInput()
{
    m_hiddenInput->clear();
    updateDisplay();
    m_submitButton->setEnabled(false);
}

void GuessInputDialog::keyPressEvent(QKeyEvent *event)
{
    // 确保输入框始终有焦点
    if (!m_hiddenInput->hasFocus()) {
        m_hiddenInput->setFocus();
    }
    
    // 让基类处理键盘事件(这样输入会传递到QLineEdit)
    QDialog::keyPressEvent(event);
}

void GuessInputDialog::onTextChanged(const QString &text)
{
    updateDisplay();

    // 允许任意长度的输入，只要不为空就可以提交
    m_submitButton->setEnabled(!text.trimmed().isEmpty());
}

void GuessInputDialog::onSubmitClicked()
{
    QString guess = m_hiddenInput->text().trimmed();
    if (!guess.isEmpty()) {
        qDebug() << "提交猜测:" << guess;
        emit guessSubmitted(guess);
        // 清空输入并重新聚焦,允许继续输入
        m_hiddenInput->clear();
        m_submitButton->setEnabled(false); // 禁用提交按钮直到有新输入
        updateDisplay();
        // 使用 QTimer 延迟设置焦点,确保对话框准备好
        QTimer::singleShot(50, m_hiddenInput, [this]() {
            m_hiddenInput->setFocus();
        });
        qDebug() << "输入框已清空,等待下次输入";
    }
}

void GuessInputDialog::updateDisplay()
{
    QString displayText;
    QString inputText = m_hiddenInput->text();

    // 显示所有已输入的文字，用空格分隔每个字符
    for (int i = 0; i < inputText.length(); ++i) {
        displayText += inputText[i];
        if (i < inputText.length() - 1) {
            displayText += " ";
        }
    }
    
    // 如果没有输入，显示提示
    if (displayText.isEmpty()) {
        displayText = "请输入你的猜测... (回车提交)";
    }

    m_displayLabel->setText(displayText);
}

GameTimer::GameTimer(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_gameActive(false)
    , m_timeoutPopupShown(false)
    , m_inputDialog(nullptr)
    , m_parentWidget(nullptr)
    , m_isDrawer(false)
{
    connect(m_timer, &QTimer::timeout, this, &GameTimer::onTimerTimeout);
}

void GameTimer::stopTimer()
{
    m_gameActive = false;
    m_timer->stop();
    m_timeoutPopupShown = false;
    
    if (m_inputDialog) {
        m_inputDialog->close();
        m_inputDialog->deleteLater();
        m_inputDialog = nullptr;
    }
}

void GameTimer::startGuessGame(int durationSeconds, const QString& targetWord, QWidget *parentWidget, bool isDrawer)
{
    m_targetWord = targetWord;
    m_totalDuration = durationSeconds;
    m_remainingTime = QTime(0, 0, 0).addSecs(durationSeconds);
    m_gameActive = true;
    m_timeoutPopupShown = false;
    m_lastGuess.clear();
    m_parentWidget = parentWidget; // 保存父窗口
    m_isDrawer = isDrawer; // 设置画师标识

    qDebug() << "游戏开始！你有" << durationSeconds << "秒来猜单词";
    qDebug() << "目标单词长度:" << targetWord.length() << "个字母";
    qDebug() << "玩家身份:" << (isDrawer ? "画师" : "猜词者");

    m_timer->start(100);

    // 只有猜词者才显示输入对话框
    if (!isDrawer) {
        showInputDialog();
    }
}

void GameTimer::startTimer(int durationSeconds, bool isDrawer)
{
    m_totalDuration = durationSeconds;
    m_remainingTime = QTime(0, 0, 0).addSecs(durationSeconds);
    m_gameActive = true;
    m_timeoutPopupShown = false;
    m_lastGuess.clear();
    m_targetWord.clear();
    m_parentWidget = nullptr;
    m_isDrawer = isDrawer; // 设置画师标识

    qDebug() << "后台计时器开始,时长:" << durationSeconds << "秒";
    qDebug() << "玩家身份:" << (isDrawer ? "画师" : "猜词者");

    m_timer->start(100);
    // 不显示对话框
}

void GameTimer::onTimerTimeout()
{
    if (!m_gameActive) {
        m_timer->stop();
        if (m_inputDialog) {
            m_inputDialog->close();
            m_inputDialog->deleteLater();
            m_inputDialog = nullptr;
        }
        return;
    }

    m_remainingTime = m_remainingTime.addMSecs(-100);

    if (m_inputDialog && m_inputDialog->isVisible()) {
        int remainingSecs = QTime(0, 0, 0).secsTo(m_remainingTime);
        m_inputDialog->setRemainingTime(remainingSecs);
    }

    if (m_remainingTime <= QTime(0, 0, 0)) {
        m_gameActive = false;
        m_timer->stop();

        qDebug() << "\n=== 时间到！游戏结束 ===";
        qDebug() << "目标单词是:" << m_targetWord;

        // 输出最后一次猜测（如果有），否则提示未猜测
        if (!m_lastGuess.isEmpty()) {
            qDebug() << "你的最后一次猜测是:" << m_lastGuess;
        } else {
            qDebug() << "你一次都没有猜测！";
        }

        if (m_inputDialog) {
            m_inputDialog->close();
            m_inputDialog->deleteLater();
            m_inputDialog = nullptr;
        }

        // 只有非画师玩家才显示弹窗，并且保证只显示一次
        if (!m_timeoutPopupShown && !m_isDrawer) {
            m_timeoutPopupShown = true;
            QMessageBox::information(nullptr, "游戏结束",
                                    QString("目标词语: %1").arg(m_targetWord));
        }
        
        emit timeUp(); // 发出时间到信号
    }
}

void GameTimer::showInputDialog()
{
    if (!m_gameActive) return;

    if (m_inputDialog && m_inputDialog->isVisible()) {
        m_inputDialog->clearInput();
        m_inputDialog->activateWindow();
        m_inputDialog->raise(); // 确保窗口在最前面
        return;
    }

    if (m_inputDialog) {
        m_inputDialog->deleteLater();
    }

    m_inputDialog = new GuessInputDialog(m_targetWord.length(), m_parentWidget);
    m_inputDialog->setAttribute(Qt::WA_DeleteOnClose);
    
    // 设置窗口标志，确保对话框始终在最前面
    m_inputDialog->setWindowFlags(m_inputDialog->windowFlags() | Qt::WindowStaysOnTopHint);
    
    // 如果有父窗口，将对话框居中显示在父窗口上
    if (m_parentWidget) {
        QRect parentGeometry = m_parentWidget->geometry();
        int x = parentGeometry.x() + (parentGeometry.width() - m_inputDialog->width()) / 2;
        int y = parentGeometry.y() + (parentGeometry.height() - m_inputDialog->height()) / 2;
        m_inputDialog->move(x, y);
        qDebug() << "对话框将显示在父窗口中央位置:" << x << y;
    }

    int remainingSecs = QTime(0, 0, 0).secsTo(m_remainingTime);
    m_inputDialog->setRemainingTime(remainingSecs);

    connect(m_inputDialog, &GuessInputDialog::guessSubmitted, [this](const QString& guess) {
        if (!m_gameActive) return;

        m_lastGuess = guess;
        
        qDebug() << "玩家猜测:" << guess;
        
        // 发出信号,让MainWindow发送给服务器验证
        emit guessSubmitted(guess);
        
        // 不在这里关闭对话框,等待服务器的猜对消息
    });

    connect(m_inputDialog, &QDialog::rejected, [this]() {
        if (m_gameActive) {
            QTimer::singleShot(100, this, &GameTimer::showInputDialog);
        }
    });

    m_inputDialog->show();
    m_inputDialog->activateWindow();
    m_inputDialog->raise(); // 确保显示在最前面
    
    qDebug() << "计时器对话框已显示";
}