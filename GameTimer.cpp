#include "GameTimer.h"

//initialize the dialogwindow and interaction
GuessInputDialog::GuessInputDialog(int wordLength, QWidget *parent)
    : QDialog(parent), m_wordLength(wordLength)
{
    setWindowTitle("猜单词");
    setModal(false);
    //player can interact at other windows
    setFixedSize(400, 200);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    //dynamically update timer
    m_timeLabel = new QLabel("剩余时间: -- 秒", this);
    mainLayout->addWidget(m_timeLabel);

    m_displayLabel = new QLabel(this);
    QFont displayFont("Monospace", 16, QFont::Bold);
    m_displayLabel->setFont(displayFont);
    m_displayLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_displayLabel);
    //core part for diasplaying
    m_hiddenInput = new QLineEdit(this);
    // remove the restriction for length
    // m_hiddenInput->setMaxLength(m_wordLength);
    m_hiddenInput->setVisible(false);

    m_submitButton = new QPushButton("提交答案", this);
    m_submitButton->setEnabled(false);

    //create the boutton of"提交答案“,and only be active when input something
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_submitButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    //show the button in fixed position
    connect(m_hiddenInput, &QLineEdit::textChanged, this, &GuessInputDialog::onTextChanged);
    connect(m_submitButton, &QPushButton::clicked, this, &GuessInputDialog::onSubmitClicked);

    //submit input
    connect(m_hiddenInput, &QLineEdit::returnPressed, this, &GuessInputDialog::onSubmitClicked);

    updateDisplay();

    m_hiddenInput->setFocus();
}

//update the remainingtime and show it
void GuessInputDialog::setRemainingTime(int seconds)
{
    m_timeLabel->setText(QString("剩余时间: %1 秒").arg(seconds));
}

//get guess txt
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
    // make sure there has focus on keyboard
    if (!m_hiddenInput->hasFocus()) {
        m_hiddenInput->setFocus();
    }

    QDialog::keyPressEvent(event);
}

void GuessInputDialog::onTextChanged(const QString &text)
{
    updateDisplay();

    // allow any length input except0
    m_submitButton->setEnabled(!text.trimmed().isEmpty());
}

void GuessInputDialog::onSubmitClicked()
{
    QString guess = m_hiddenInput->text().trimmed();
    if (!guess.isEmpty()) {
        qDebug() << "提交猜测:" << guess;
        emit guessSubmitted(guess);
        // clear and continue
        m_hiddenInput->clear();
        m_submitButton->setEnabled(false); // forbid submit until new iuput
        updateDisplay();
        // make sure that prepared for next input
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

    // add" "between every characters
    for (int i = 0; i < inputText.length(); ++i) {
        displayText += inputText[i];
        if (i < inputText.length() - 1) {
            displayText += " ";
        }
    }

    // hint
    if (displayText.isEmpty()) {
        displayText = "请输入你的猜测... (回车提交)";
    }

    m_displayLabel->setText(displayText);
}

//initialize
GameTimer::GameTimer(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_gameActive(false)
    , m_timeoutPopupShown(false)
    , m_inputDialog(nullptr)
    , m_parentWidget(nullptr)
{
    connect(m_timer, &QTimer::timeout, this, &GameTimer::onTimerTimeout);
}

//stop timer and clear the source
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
    //initialize the game status
    m_targetWord = targetWord;
    m_totalDuration = durationSeconds;
    m_remainingTime = QTime(0, 0, 0).addSecs(durationSeconds);
    m_gameActive = true;
    m_timeoutPopupShown = false;
    m_lastGuess.clear();
    m_parentWidget = parentWidget;

    qDebug() << "游戏开始！你有" << durationSeconds << "秒来猜单词";
    qDebug() << "目标单词长度:" << targetWord.length() << "个字母";

    m_timer->start(100);

    showInputDialog();
}

void GameTimer::startTimer(int durationSeconds, bool isDrawer)
{
    //with no objective words or parentwindow
    m_totalDuration = durationSeconds;
    m_remainingTime = QTime(0, 0, 0).addSecs(durationSeconds);
    m_gameActive = true;
    m_timeoutPopupShown = false;
    m_lastGuess.clear();
    m_targetWord.clear();
    m_parentWidget = nullptr;

    qDebug() << "后台计时器开始,时长:" << durationSeconds << "秒";

    m_timer->start(100);
}

void GameTimer::onTimerTimeout()
{
    if (!m_gameActive) {//not active
        m_timer->stop();
        if (m_inputDialog) {
            m_inputDialog->close();
            m_inputDialog->deleteLater();
            m_inputDialog = nullptr;
        }
        return;
    }

    m_remainingTime = m_remainingTime.addMSecs(-100);

    //update the display of the remaining time
    if (m_inputDialog && m_inputDialog->isVisible()) {
        int remainingSecs = QTime(0, 0, 0).secsTo(m_remainingTime);
        m_inputDialog->setRemainingTime(remainingSecs);
    }

    if (m_remainingTime <= QTime(0, 0, 0)) {
        m_gameActive = false;
        m_timer->stop();

        qDebug() << "\n=== 时间到！游戏结束 ===";
        qDebug() << "目标单词是:" << m_targetWord;

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

        // avoid being quit by window
        emit timeUp();

        // 只有猜词者（有targetWord）才弹窗，画手的后台计时器不弹窗
        // 使用 QTimer::singleShot 让弹窗异步显示，不阻塞信号处理
        if (!m_targetWord.isEmpty() && !m_timeoutPopupShown) {
            m_timeoutPopupShown = true;
            QString word = m_targetWord;
            QString guess = m_lastGuess;
            QTimer::singleShot(0, [word, guess]() {
                QMessageBox::information(nullptr, "游戏结束",
                                         QString("时间到！\n目标单词: %1\n你的最后一次猜测: %2")
                                             .arg(word)
                                             .arg(guess.isEmpty() ? "无" : guess));
            });
        }
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

    // set the window logo to make sure that the dialog is at top
    m_inputDialog->setWindowFlags(m_inputDialog->windowFlags() | Qt::WindowStaysOnTopHint);

    // in the center of parent window
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

        // send signal for websocket for checking
        emit guessSubmitted(guess);

        // close the window until receiving the message from the websocket
    });

    connect(m_inputDialog, &QDialog::rejected, [this]() {
        if (m_gameActive) {
            QTimer::singleShot(100, this, &GameTimer::showInputDialog);
        }
    });

    m_inputDialog->show();
    m_inputDialog->activateWindow();
    m_inputDialog->raise();

    qDebug() << "计时器对话框已显示";
}
