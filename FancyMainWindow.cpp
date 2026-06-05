#include "FancyMainWindow.h"
#include "canvas.h"
#include "connect.h"
#include "GameTimer.h"
#include "src/Fancy/PushButton.h"
#include "src/Fancy/SidebarButton.h"
#include "src/Fancy/Sidebar.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColorDialog>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

FancyMainWindow::FancyMainWindow(QWidget *parent)
    : fancy::StackWindow(parent)
    , m_canvasPage(new QWidget(this))
    , m_settingsPage(new QWidget(this))
    , m_canvas(nullptr)
    , m_conn(nullptr)
    , m_log(nullptr)
    , m_gameTimer(nullptr)
{
    setWindowTitle("你画我猜 - 云母效果版");
    resize(1200, 800);
    
    setupUI();
    setupSidebar();
    
    // 连接管理
    m_conn = new ConnectManager(this);
    
    // 游戏计时器
    m_gameTimer = new GameTimer(this);
    
    setupConnections();
    wireUiSignals();
}

FancyMainWindow::~FancyMainWindow()
{
}

void FancyMainWindow::setupUI()
{
    // ========== 画布页面 ==========
    QHBoxLayout *canvasMainLayout = new QHBoxLayout(m_canvasPage);
    canvasMainLayout->setContentsMargins(0, 0, 0, 0);
    canvasMainLayout->setSpacing(10);
    
    // 画布容器
    QWidget *canvasContainer = new QWidget();
    canvasContainer->setMinimumSize(700, 600);
    m_canvas = new Canvas(canvasContainer);
    m_canvas->setGeometry(0, 0, 700, 600);
    m_canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 初始状态：禁止绘制
    m_canvas->setRoundActive(false);
    m_canvas->setMyTurn(false);
    m_canvas->setDrawer(false);
    
    // 右侧控制面板
    QWidget *controlPanel = new QWidget();
    controlPanel->setMaximumWidth(250);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
    
    // 颜色按钮
    m_colorButton = new fancy::PushButton("选择颜色");
    m_colorButton->setMinimumHeight(40);
    controlLayout->addWidget(m_colorButton);
    
    // 橡皮擦按钮
    m_eraserButton = new fancy::PushButton("橡皮擦");
    m_eraserButton->setMinimumHeight(40);
    m_eraserButton->setCheckable(true);
    controlLayout->addWidget(m_eraserButton);
    
    // 清空画布按钮
    m_clearButton = new fancy::PushButton("清空画布");
    m_clearButton->setMinimumHeight(40);
    controlLayout->addWidget(m_clearButton);
    
    // 画笔粗细
    QLabel *widthLabel = new QLabel("画笔粗细:");
    controlLayout->addWidget(widthLabel);
    
    m_penWidthSlider = new QSlider(Qt::Horizontal);
    m_penWidthSlider->setMinimum(1);
    m_penWidthSlider->setMaximum(20);
    m_penWidthSlider->setValue(3);
    controlLayout->addWidget(m_penWidthSlider);
    
    // 连接控制
    QLabel *connectLabel = new QLabel("连接控制:");
    controlLayout->addWidget(connectLabel);
    
    QLineEdit *serverUrlEdit = new QLineEdit();
    serverUrlEdit->setObjectName("serverUrl");
    serverUrlEdit->setPlaceholderText("ws://服务器地址:端口");
    serverUrlEdit->setText("ws://localhost:8080");
    controlLayout->addWidget(serverUrlEdit);
    
    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setObjectName("nameEdit");
    nameEdit->setPlaceholderText("输入昵称");
    controlLayout->addWidget(nameEdit);
    
    QPushButton *connectBtn = new QPushButton("连接");
    connectBtn->setObjectName("btnConnect");
    controlLayout->addWidget(connectBtn);
    
    QPushButton *disconnectBtn = new QPushButton("断开");
    disconnectBtn->setObjectName("btnDisconnect");
    disconnectBtn->setEnabled(false);
    controlLayout->addWidget(disconnectBtn);
    
    // 房间控制
    QLabel *roomLabel = new QLabel("房间控制:");
    controlLayout->addWidget(roomLabel);
    
    QLineEdit *roomEdit = new QLineEdit();
    roomEdit->setObjectName("roomEdit");
    roomEdit->setPlaceholderText("房间号");
    controlLayout->addWidget(roomEdit);
    
    QPushButton *createRoomBtn = new QPushButton("创建房间");
    createRoomBtn->setObjectName("btnCreateRoom");
    controlLayout->addWidget(createRoomBtn);
    
    QPushButton *joinRoomBtn = new QPushButton("加入房间");
    joinRoomBtn->setObjectName("btnJoinRoom");
    controlLayout->addWidget(joinRoomBtn);
    
    QPushButton *startGameBtn = new QPushButton("开始游戏");
    startGameBtn->setObjectName("btnStartGame");
    controlLayout->addWidget(startGameBtn);
    
    // 聊天/猜词
    QLabel *chatLabel = new QLabel("聊天/猜词:");
    controlLayout->addWidget(chatLabel);
    
    QLineEdit *chatEdit = new QLineEdit();
    chatEdit->setObjectName("chatEdit");
    chatEdit->setPlaceholderText("聊天消息");
    chatEdit->setEnabled(false);
    controlLayout->addWidget(chatEdit);
    
    QLineEdit *guessEdit = new QLineEdit();
    guessEdit->setObjectName("guessEdit");
    guessEdit->setPlaceholderText("猜测词语");
    guessEdit->setEnabled(false);
    controlLayout->addWidget(guessEdit);
    
    controlLayout->addStretch();
    
    // 组装画布页面
    canvasMainLayout->addWidget(canvasContainer, 3);
    canvasMainLayout->addWidget(controlPanel, 1);
    
    // 日志区域
    m_log = new QTextBrowser();
    m_log->setObjectName("messageLog");
    m_log->setMaximumHeight(150);
    m_log->setPlaceholderText("消息日志...");
    
    QWidget *canvasWithLog = new QWidget();
    QVBoxLayout *canvasWithLogLayout = new QVBoxLayout(canvasWithLog);
    canvasWithLogLayout->setContentsMargins(10, 10, 10, 10);
    canvasWithLogLayout->addWidget(m_canvasPage);
    canvasWithLogLayout->addWidget(m_log);
    
    // ========== 设置页面 ==========
    QVBoxLayout *settingsLayout = new QVBoxLayout(m_settingsPage);
    QLabel *settingsLabel = new QLabel("设置页面");
    settingsLabel->setAlignment(Qt::AlignCenter);
    settingsLayout->addWidget(settingsLabel);
    settingsLayout->addStretch();
    
    // 添加页面到 StackWidget
    addWidget(canvasWithLog);
    addWidget(m_settingsPage);
    
    setCurrentIndex(0);
    
    // 初始禁用控制按钮
    updateControlButtons(false);
    updateHostButtons(false);
}

void FancyMainWindow::setupSidebar()
{
    // 创建侧边栏按钮
    m_homeBtn = new fancy::SidebarButton(fancy::IconPark::Home, "画布", this);
    
    m_settingsBtn = new fancy::SidebarButton(fancy::IconPark::SettingTwo, "设置", this);
    
    // 添加到侧边栏
    addSidebarOption(m_homeBtn);
    addSidebarOption(m_settingsBtn);
    
    // 连接信号
    connect(m_homeBtn, &fancy::SidebarButton::clicked, this, [this]() {
        setCurrentIndex(0);
    });
    
    connect(m_settingsBtn, &fancy::SidebarButton::clicked, this, [this]() {
        setCurrentIndex(1);
    });
    
    // 默认选中画布
    m_homeBtn->setChecked(true);
}

void FancyMainWindow::setupConnections()
{
    connect(m_conn, &ConnectManager::log, this, &FancyMainWindow::addLog);
    connect(m_conn, &ConnectManager::errorOccurred, this, [this](const QString& e){ 
        addLog(QString::fromUtf8("❌ ") + e); 
    });

    connect(m_conn, &ConnectManager::connected, this, [this](){
        addLog(QString::fromUtf8("已连接服务器"));
        if (auto btn = findChild<QPushButton*>("btnDisconnect")) btn->setEnabled(true);
        if (auto btn = findChild<QPushButton*>("btnConnect")) btn->setEnabled(false);
        if (auto le = findChild<QLineEdit*>("chatEdit")) le->setEnabled(true);
        if (auto le = findChild<QLineEdit*>("guessEdit")) le->setEnabled(true);
        
        m_isHost = false;
        m_playerId.clear();
        updateHostButtons(false);
        
        // 自动加入游戏
        if (!m_playerName.isEmpty()) {
            QJsonObject join{{"type", 1}, {"username", m_playerName}};
            m_conn->sendText(QString::fromUtf8(QJsonDocument(join).toJson(QJsonDocument::Compact)));
        }
    });
    
    connect(m_conn, &ConnectManager::disconnected, this, [this](){
        addLog(QString::fromUtf8("已断开服务器"));
        if (auto btn = findChild<QPushButton*>("btnDisconnect")) btn->setEnabled(false);
        if (auto btn = findChild<QPushButton*>("btnConnect")) btn->setEnabled(true);
        onGameEnded();
        m_isHost = false;
        m_playerId.clear();
        updateHostButtons(false);
    });
    
    // 绘图 -> 网络
    connect(m_canvas, &Canvas::strokeBegan, this, [this](const Stroke& s){
        QJsonObject obj{
            {"type", 3},
            {"action", "begin"},
            {"color", colorToHex(s.color)},
            {"width", s.width},
            {"eraser", s.eraser}
        };
        m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
    });

    connect(m_canvas, &Canvas::strokePointAdded, this, [this](const QPoint& pt){
        QJsonObject obj{{"type", 3}, {"action", "point"}, {"x", pt.x()}, {"y", pt.y()}};
        m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
    });

    connect(m_canvas, &Canvas::strokeEnded, this, [this](){
        QJsonObject obj{{"type", 3}, {"action", "end"}};
        m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
    });
    
    // 计时器信号
    connect(m_gameTimer, &GameTimer::timeUp, this, [this]() {
        addLog("计时结束！回合结束");
        if (m_isHost) {
            QJsonObject obj{{"type", 31}};
            m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
        }
        onRoundEnded();
    });
    
    connect(m_gameTimer, &GameTimer::guessSubmitted, this, [this](const QString& guess) {
        QJsonObject obj{{"type", 5}, {"guess", guess}};
        m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
        addLog(QString("已提交猜测: %1").arg(guess));
    });
}

void FancyMainWindow::wireUiSignals()
{
    // 颜色选择
    connect(m_colorButton, &fancy::PushButton::clicked, this, [this]() {
        QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");
        if (color.isValid()) {
            m_canvas->setColor(color);
            m_eraserButton->setChecked(false);
        }
    });
    
    // 橡皮擦
    connect(m_eraserButton, &fancy::PushButton::toggled, this, [this](bool checked) {
        m_canvas->setEraser(checked);
    });
    
    // 清空画布
    connect(m_clearButton, &fancy::PushButton::clicked, m_canvas, &Canvas::clearCanvas);
    
    // 画笔粗细
    connect(m_penWidthSlider, &QSlider::valueChanged, m_canvas, &Canvas::setWidth);
    
    // 连接按钮
    if (auto btn = findChild<QPushButton*>("btnConnect")) {
        connect(btn, &QPushButton::clicked, this, [this]() {
            auto urlEdit = findChild<QLineEdit*>("serverUrl");
            auto nameEdit = findChild<QLineEdit*>("nameEdit");
            if (urlEdit && nameEdit) {
                m_playerName = nameEdit->text().trimmed();
                if (m_playerName.isEmpty()) {
                    addLog("请输入昵称");
                    return;
                }
                m_conn->connectTo(QUrl(urlEdit->text()));
            }
        });
    }
    
    if (auto btn = findChild<QPushButton*>("btnDisconnect")) {
        connect(btn, &QPushButton::clicked, m_conn, &ConnectManager::disconnectFrom);
    }
    
    // 房间按钮
    if (auto btn = findChild<QPushButton*>("btnCreateRoom")) {
        connect(btn, &QPushButton::clicked, this, [this]() {
            auto roomEdit = findChild<QLineEdit*>("roomEdit");
            if (roomEdit) {
                QString roomId = roomEdit->text().trimmed();
                if (roomId.isEmpty()) {
                    addLog("请输入房间号");
                    return;
                }
                QJsonObject obj{{"type", 10}, {"room_id", roomId}};
                m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
            }
        });
    }
    
    if (auto btn = findChild<QPushButton*>("btnJoinRoom")) {
        connect(btn, &QPushButton::clicked, this, [this]() {
            auto roomEdit = findChild<QLineEdit*>("roomEdit");
            if (roomEdit) {
                QString roomId = roomEdit->text().trimmed();
                if (roomId.isEmpty()) {
                    addLog("请输入房间号");
                    return;
                }
                QJsonObject obj{{"type", 11}, {"room_id", roomId}};
                m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
            }
        });
    }
    
    if (auto btn = findChild<QPushButton*>("btnStartGame")) {
        connect(btn, &QPushButton::clicked, this, [this]() {
            QJsonObject obj{{"type", 20}};
            m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
            addLog("已发送开始游戏请求");
        });
    }
    
    // 聊天
    if (auto edit = findChild<QLineEdit*>("chatEdit")) {
        connect(edit, &QLineEdit::returnPressed, this, [this, edit]() {
            QString text = edit->text().trimmed();
            if (!text.isEmpty()) {
                QJsonObject obj{{"type", 4}, {"message", text}};
                m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
                edit->clear();
            }
        });
    }
    
    // 猜词
    if (auto edit = findChild<QLineEdit*>("guessEdit")) {
        connect(edit, &QLineEdit::returnPressed, this, [this, edit]() {
            QString text = edit->text().trimmed();
            if (!text.isEmpty()) {
                QJsonObject obj{{"type", 5}, {"guess", text}};
                m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
                edit->clear();
                addLog(QString("已提交猜测: %1").arg(text));
            }
        });
    }
}

void FancyMainWindow::addLog(const QString& msg)
{
    if (m_log) {
        m_log->append(msg);
    }
}

QString FancyMainWindow::colorToHex(const QColor& c) const
{
    return QString("#%1%2%3")
        .arg(c.red(), 2, 16, QChar('0'))
        .arg(c.green(), 2, 16, QChar('0'))
        .arg(c.blue(), 2, 16, QChar('0'));
}

void FancyMainWindow::updateControlButtons(bool isDrawer)
{
    m_colorButton->setEnabled(isDrawer);
    m_eraserButton->setEnabled(isDrawer);
    m_clearButton->setEnabled(isDrawer);
    m_penWidthSlider->setEnabled(isDrawer);
}

void FancyMainWindow::updateHostButtons(bool isHost)
{
    if (auto btn = findChild<QPushButton*>("btnStartGame")) {
        btn->setEnabled(isHost);
    }
}

void FancyMainWindow::onRoundStarted(bool isDrawer)
{
    m_isDrawer = isDrawer;
    m_canvas->setRoundActive(true);
    m_canvas->setMyTurn(isDrawer);
    m_canvas->setDrawer(isDrawer);
    updateControlButtons(isDrawer);
    
    if (isDrawer) {
        addLog("你是画手，开始绘画！");
    } else {
        addLog("你是猜词者，开始猜测！");
    }
    
    m_roundStartTime = QTime::currentTime();
}

void FancyMainWindow::onRoundEnded()
{
    m_canvas->setRoundActive(false);
    m_canvas->setMyTurn(false);
    m_canvas->setDrawer(false);
    updateControlButtons(false);
    m_isDrawer = false;
    
    if (m_gameTimer) {
        m_gameTimer->stopTimer();
    }
}

void FancyMainWindow::onDrawerAssigned(const QString &drawerName)
{
    bool amIDrawer = (drawerName == m_playerId || drawerName == m_playerName);
    onRoundStarted(amIDrawer);
}

void FancyMainWindow::onGameEnded()
{
    onRoundEnded();
    m_canvas->clearCanvas();
    addLog("游戏已结束");
}

void FancyMainWindow::onGuessCorrect()
{
    if (!m_isDrawer) {
        addLog("恭喜！你猜对了！");
    }
}
