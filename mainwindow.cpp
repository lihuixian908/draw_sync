#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "canvas.h"
#include "connect.h"
#include "GameTimer.h"
#include "SqliteTools.h"
#include "ChatWindow.h"

#include <QColorDialog>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QUrl>
#include <QTime>
#include <QDebug>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QResizeEvent>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

#include "FancyButton.h"

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windowsx.h>
#pragma comment(lib, "dwmapi.lib")

// Windows 11 云母效果相关常量
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMSBT_MAINWINDOW
#define DWMSBT_MAINWINDOW 2  // Mica 效果
#endif
#ifndef DWMSBT_TRANSIENTWINDOW
#define DWMSBT_TRANSIENTWINDOW 3  // Acrylic 效果
#endif
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_canvas(nullptr)
{
    ui->setupUi(this);
    
    // 设置窗口最小尺寸
    setMinimumSize(800, 600);
    
    // 启用窗口最大化按钮
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

    // 画布
    m_canvas = new Canvas(ui->canvasWidget);
    m_canvas->setGeometry(ui->canvasWidget->rect());
    m_canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 初始状态：禁止绘制
    m_canvas->setRoundActive(false);
    m_canvas->setMyTurn(false);
    m_canvas->setDrawer(false);

    // 初始禁用所有控制按钮
    updateControlButtons(false);
    updateHostButtons(false);
    
    // 初始化画手词显示
    clearWordLabel();

    // 日志（若 ui 中没有 messageLog，则动态创建一个）
    m_log = findChild<QTextBrowser*>(QStringLiteral("messageLog"));
    if (!m_log) {
        m_log = new QTextBrowser(this);
        m_log->setObjectName(QStringLiteral("messageLog"));
        // 初始位置会在resizeEvent中调整
        m_log->setGeometry(0, height() - 140, width() - 200, 120);
        m_log->setPlaceholderText(tr("消息日志..."));
        m_log->hide(); // 隐藏日志栏
    } else {
        m_log->hide(); // 如果已存在也隐藏
    }

    // 连接管理
    m_conn = new ConnectManager(this);

    // 游戏计时器
    m_gameTimer = new GameTimer(this);
    
    // 数据库处理器
    m_dbHandler = new DBHandler("./game_process.db", this);
    
    // 创建排行榜表格（初始隐藏）
    m_leaderboardTable = new QTableWidget(this);
    m_leaderboardTable->setColumnCount(4);
    m_leaderboardTable->setHorizontalHeaderLabels({"排名", "玩家", "分数", "正确"});
    m_leaderboardTable->setGeometry(200, 100, 500, 400);
    m_leaderboardTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_leaderboardTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_leaderboardTable->horizontalHeader()->setStretchLastSection(true);
    m_leaderboardTable->setStyleSheet(R"(
        QTableWidget {
            background-color: rgba(255, 255, 255, 240);
            border: 2px solid #2196F3;
            border-radius: 10px;
        }
        QHeaderView::section {
            background-color: #2196F3;
            color: white;
            padding: 5px;
            font-weight: bold;
        }
    )");
    m_leaderboardTable->hide();
    
    // 创建聊天窗口（初始隐藏）
    m_chatWindow = new ChatWindow(this);
    connect(m_chatWindow, &ChatWindow::sendChatMessage, this, &MainWindow::onSendChatMessage);
    
    // 创建连接状态标签
    m_connectionStatusLabel = new QLabel(this);
    m_connectionStatusLabel->setAlignment(Qt::AlignCenter);
    m_connectionStatusLabel->setStyleSheet(R"(
        QLabel {
            padding: 4px 10px;
            border-radius: 4px;
            font-weight: bold;
            font-size: 11px;
        }
    )");
    m_connectionStatusLabel->setGeometry(width() - 160, 10, 150, 28);  // 
    updateConnectionStatus(false, "未连接");
    m_connectionStatusLabel->show();
    
    // 初始化UI中的房间状态标签
    if (auto label = findChild<QLabel*>("roomStatusLabel")) {
        label->setText("未加入房间");
    }
    
    // 创建房间列表容器
    m_roomListWidget = new QWidget(this);
    m_roomListWidget->setGeometry(200, 150, 400, 350);
    m_roomListWidget->setStyleSheet(R"(
        QWidget {
            background-color: rgba(255, 255, 255, 240);
            border: 2px solid #3498db;
            border-radius: 10px;
        }
    )");
    m_roomListWidget->hide();
    
    // 连接计时器信号
    connect(m_gameTimer, &GameTimer::timeUp, this, [this]() {
        qDebug() << "*** 倒计时结束 ***";
        
        // 所有玩家在倒计时结束时都发送回合结束请求给服务器
        // 服务器会处理并广播回合结束消息给所有玩家
        QJsonObject obj{{"type", 31}}; // type 31 = 回合结束
        m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
        qDebug() << "倒计时结束，发送回合结束消息给服务器";
        
        // 本地立即结束回合
        onRoundEnded();
    });
    
    connect(m_gameTimer, &GameTimer::wordGuessed, this, [this]() {
        addLog("恭喜猜对词语！");
        // 猜对后不需要再处理,因为服务器会发送猜对消息
        // onGuessCorrect();
    });
    
    // 连接计时器的猜测提交信号,发送给服务器
    connect(m_gameTimer, &GameTimer::guessSubmitted, this, [this](const QString& guess) {
        qDebug() << "从计时器收到猜测,发送给服务器:" << guess;
        // 发送猜测到服务器
        QJsonObject obj{{"type", 5}, {"guess", guess}};
        m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
        addLog(QString("已提交猜测: %1").arg(guess));
    });

    setupConnections();
    wireUiSignals();

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
        qDebug() << "发送绘制开始:" << obj;
    });

    connect(m_canvas, &Canvas::strokePointAdded, this, [this](const QPoint& pt){
        QJsonObject obj{{"type", 3}, {"action", "point"}, {"x", pt.x()}, {"y", pt.y()}};
        m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
    });

    connect(m_canvas, &Canvas::strokeEnded, this, [this](){
        QJsonObject obj{{"type", 3}, {"action", "end"}};
        m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
        qDebug() << "发送绘制结束";
    });
    
    // 启用云母效果
    enableMicaEffect();
}

MainWindow::~MainWindow()
{
    // Release the UI resources created by Qt Designer
    delete ui;
}

void MainWindow::setupConnections()
{
    // Bind ConnectManager's log signal to MainWindow's addLog slot
    connect(m_conn, &ConnectManager::log, this, &MainWindow::addLog);

    //  Handle connection error: update status, log error, disable related UI
    connect(m_conn, &ConnectManager::errorOccurred, this, [this](const QString& e){
        updateConnectionStatus(false, "Connection Failed");
        addLog(QString::fromUtf8("❌ ") + e);
    });

    // Handle successful connection to WebSocket server
    connect(m_conn, &ConnectManager::connected, this, [this](){
        // Update UI status
        updateConnectionStatus(true, "Connected");
        addLog(QString::fromUtf8("✅ Connected to server"));

        // Enable disconnect button, disable connect button
        if (auto btn = findChild<QPushButton*>("btnDisconnect")) btn->setEnabled(true);
        if (auto btn = findChild<QPushButton*>("btnConnect")) btn->setEnabled(false);
        // Enable chat and guess input fields after connection
        if (auto le = findChild<QLineEdit*>("chatEdit")) le->setEnabled(true);
        if (auto le = findChild<QLineEdit*>("guessEdit")) le->setEnabled(true);

        // Reset game state: clear host flag and player ID
        m_isHost = false;
        m_playerId.clear();
        updateHostButtons(false);

        // Auto-join game if player name is set
        if (!m_playerName.isEmpty()) {
            // Construct join game request (JSON format: type=1 for join, with username)
            QJsonObject join{{"type", 1}, {"username", m_playerName}};
            // Send compact JSON string via WebSocket (reduce data size)
            m_conn->sendText(QString::fromUtf8(QJsonDocument(join).toJson(QJsonDocument::Compact)));

            // Initialize player score record in local database (if DB is available)
            if (m_dbHandler && m_dbHandler->isValid()) {
                PlayerScoreInfo info;
                // Create new record if player is first-time login
                if (!m_dbHandler->getPlayerScore(m_playerName, info)) {
                    m_dbHandler->insertNewPlayer(m_playerName, 0, 0, 0);
                    qDebug() << "Created new score record for player:" << m_playerName;
                }
            }
        }
    });

    // Handle disconnection from WebSocket server (active/inactive)
    connect(m_conn, &ConnectManager::disconnected, this, [this](){
        updateConnectionStatus(false, "Disconnected");
        addLog(QString::fromUtf8("⚠️ Disconnected from server"));

        // Reset button states: disable disconnect, enable connect
        if (auto btn = findChild<QPushButton*>("btnDisconnect")) btn->setEnabled(false);
        if (auto btn = findChild<QPushButton*>("btnConnect")) btn->setEnabled(true);

        // Reset game state on disconnection: stop drawing/guessing, clear room info
        onGameEnded(); // Clean up canvas, timer, and game UI
        m_isHost = false; // Revoke host privilege
        m_playerId.clear(); // Clear player ID (invalid after disconnection)
        m_currentRoom.clear(); // Clear current room ID (no longer valid)
        updateRoomInfoDisplay("", ""); // Hide room info from UI
        updateHostButtons(false); // Disable host buttons
    });
//////////////////////////////////////////////////////////////////
    // handle all messages received from WebSocket server
    connect(m_conn, &ConnectManager::messageReceived, this, [this](const QString& msg){
        qDebug() << "Received message from server:" << msg;

        // Handle plain text messages
        // Case 1: Round start (old format: "round_start drawer:<player_id>")
        if (msg.startsWith("round_start drawer:")) {
            // Extract drawer ID from message
            QString drawerId = msg.mid(19);
            qDebug() << "*** Critical Message *** Round started, drawerId:" << drawerId
                     << "My playerId:" << m_playerId << "My playerName:" << m_playerName;
            // Assign drawer role and update game state
            onDrawerAssigned(drawerId);
            return;
        }

        // Case 2: Game start
        if (msg == "game_start") {
            addLog("Game started");
            // Wait for server to send round_start/drawer assignment
            return;
        }

        // Case 3: Unified round start message
        if (msg.startsWith("Round started. Drawer:")) {
            // Split message by comma to extract drawer ID and target word
            QStringList parts = msg.split(",");
            if (parts.size() >= 2) {
                // Extract drawer ID
                QString drawerPart = parts[0];
                QString drawerId = drawerPart.mid(drawerPart.indexOf("Drawer:") + 7).trimmed();

                QString wordPart = parts[1];
                QString word = wordPart.mid(wordPart.indexOf("Word:") + 5).trimmed();

                qDebug() << "*** Received unified round start message ***";
                qDebug() << "Drawer ID:" << drawerId;
                qDebug() << "Target Word:" << word;
                qDebug() << "My player_id:" << m_playerId;

                // Determine if current player is the drawer (match ID or name)
                bool isDrawer = (drawerId == m_playerId || drawerId == m_playerName);

                if (isDrawer) {
                    addLog(QString("🎨 You are the drawer! Word: %1").arg(word));
                    qDebug() << "Identity confirmed: I am the drawer";
                    // Save drawer word and display in UI
                    m_currentWord = word;
                    updateWordLabel(word);
                    onRoundStarted(true); // Start game in drawer mode
                } else {
                    addLog(QString("Drawer is: %1, you are the guesser").arg(drawerId));
                    qDebug() << "Identity confirmed: I am the guesser";
                    // Clear word display for guesser (hide target word)
                    m_currentWord.clear();
                    clearWordLabel();
                    onRoundStarted(false); // Start game in guesser mode
                }
                return;
            }
        }

        // Case 4: Guessing phase start
        if (msg == "guessing_start") {
            addLog("Guessing phase started, waiting for drawer assignment...");
            qDebug() << "Received guessing_start, waiting for server to assign new drawer";
            return;
        }

        // Case 5: Skip duplicate drawer_word plain text
        if (msg.startsWith("drawer_word:")) {
            qDebug() << "Skipping duplicate drawer_word plain text message";
            return;
        }

        // Case 6: Game end notification (multiple possible formats for compatibility)
        if (msg == "game_end" || msg.contains("game_over") || msg.contains("Game Over")) {
            addLog("🏁 Game ended");
            onGameEnded(); // Clean up game state
            return;
        }

        // Case 7: Correct guess notification (plain text)
        if (msg.startsWith("correct_guess")) {
            QString guesserName;

            // Parse format: "correct_guess player:<name> word:<word>"
            int playerIndex = msg.indexOf("player:");
            int wordIndex = msg.indexOf(" word:");

            // Validate index positions to avoid out-of-bound errors
            if (playerIndex != -1 && wordIndex != -1 && wordIndex > playerIndex) {
                // Extract guesser name between "player:" and " word:"
                guesserName = msg.mid(playerIndex + 7, wordIndex - (playerIndex + 7)).trimmed();
                qDebug() << "Parsed guesser name from correct_guess message:" << guesserName;
            }

            if (!guesserName.isEmpty()) {
                addLog(QString("Player %1 guessed the word correctly!").arg(guesserName));
                onGuessCorrect(guesserName); // Update score/UI for correct guess
            } else {
                addLog("Someone guessed correctly, but failed to parse guesser name");
                onGuessCorrect(); // Fallback: handle correct guess without name
            }
            return;
        }

        // Case 8: Incorrect guess notification (plain text)
        if (msg.startsWith("incorrect_guess") || msg.startsWith("wrong_guess")) {
            QString guesserName = "";
            int playerIndex = msg.indexOf("player:");
            if (playerIndex != -1) {
                // Extract guesser name (supports "player:<name> guess:<xxx>" or "player:<name>")
                QString remainder = msg.mid(playerIndex + 7);
                int spaceIndex = remainder.indexOf(" ");
                if (spaceIndex != -1) {
                    guesserName = remainder.left(spaceIndex).trimmed();
                } else {
                    guesserName = remainder.trimmed();
                }
            }
            if (!guesserName.isEmpty()) {
                addLog(QString("Player %1 guessed wrong").arg(guesserName));
                onGuessWrong(guesserName); // Update UI for wrong guess
            } else {
                addLog(QString("[Message] %1").arg(msg));
            }
            return;
        }

        // Case 9: Legacy correct guess message (compatibility for old server versions)
        if (msg.contains("guessed correctly") || msg.contains("correct") || msg.contains("guess_correct")) {
            addLog("Someone guessed the word correctly!");
            onGuessCorrect();
            return;
        }

        // Case 10: Legacy incorrect guess message (duplicate handling for safety)
        if (msg.startsWith("incorrect_guess") || msg.startsWith("wrong_guess")) {
            QString guesserName = "";
            int playerIndex = msg.indexOf("player:");
            if (playerIndex != -1) {
                guesserName = msg.mid(playerIndex + 7).split(" ")[0].trimmed();
            }
            if (!guesserName.isEmpty()) {
                addLog(QString("Player %1 guessed wrong").arg(guesserName));
                onGuessWrong(guesserName);
            }
            return;
        }

        // Case 11: Round end notification (plain text)
        if (msg.startsWith("round_end") || msg.contains("wait_next_round")) {
            qDebug() << "Received round end message from server";
            onRoundEnded(); // Reset round state
            return;
        }
/////////////////////////////////////////////////////////////
        //Handle JSON format messages
        QJsonParseError err{};
        QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8(), &err);

        // Only process valid JSON objects (skip invalid/array messages)
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            const QJsonObject obj = doc.object();


            // Subcase 1: Update player ID (only from connection/room responses)
            // Response types for player ID assignment:
            // - 1: Connection success
            // - 10: Room created (host)
            // - 11: Room joined (player)
            if (obj.contains("player_id") &&
                (obj.contains("response_type") &&
                 (obj.value("response_type").toInt() == 1 ||
                  obj.value("response_type").toInt() == 10 ||
                  obj.value("response_type").toInt() == 11))) {
                QString newPlayerId = obj.value("player_id").toString();
                // Initialize player ID if empty (first connection)
                if (m_playerId.isEmpty()) {
                    m_playerId = newPlayerId;
                    addLog(QString("Your player ID: %1").arg(m_playerId));
                    qDebug() << "Initialized player ID:" << m_playerId;
                }
                // Update player ID only if changed (reconnection/room switch)
                else if (m_playerId != newPlayerId) {
                    qDebug() << "Player ID updated: from" << m_playerId << "to" << newPlayerId;
                    m_playerId = newPlayerId;
                    addLog(QString("Player ID updated: %1").arg(m_playerId));
                }
            }


            // Subcase 2: Handle room creation response (only creator is host)
            if (obj.contains("response_type") && obj.value("response_type").toInt() == 10) {
                if (obj.value("status").toString() == "room_created") {
                    m_isHost = true; // Only room creator has host privileges
                    m_currentRoom = obj.value("room_id").toString();
                    QString roomName = obj.value("room_name").toString();
                    addLog(QString("You created room %1 and became host!").arg(m_currentRoom));
                    updateRoomInfoDisplay(m_currentRoom, roomName); // Show room info in UI
                    updateHostButtons(true); // Enable host-only buttons (start game, etc.)
                }
            }

            // Subcase 3: Handle room join response (joiner is regular player)
            if (obj.contains("response_type") && obj.value("response_type").toInt() == 11) {
                if (obj.value("status").toString() == "room_joined") {
                    m_isHost = false; // Joiner has no host privileges
                    m_currentRoom = obj.value("room_id").toString();
                    QString roomName = obj.value("room_name").toString();
                    addLog(QString("Joined room %1 as a regular player").arg(m_currentRoom));
                    updateRoomInfoDisplay(m_currentRoom, roomName);
                    updateHostButtons(false); // Disable host-only buttons
                    m_roomListWidget->hide(); // Hide room list after successful join
                }
            }

            // Subcase 4: Handle room list response (update available rooms in UI)
            if (obj.contains("response_type") && obj.value("response_type").toInt() == 13) {
                if (obj.value("status").toString() == "room_list") {
                    QJsonArray rooms = obj.value("rooms").toArray();
                    addLog(QString("Fetched %1 available rooms").arg(rooms.size()));
                    updateAvailableRooms(rooms); // Refresh room list widget
                }
            }


            // Subcase 5: Handle game state messages (by "type" field)
            const int type = obj.value("type").toInt(0);
            const QString typeStr = obj.value("type").toString();

            qDebug() << "Message type (int):" << type << "Message type (string):" << typeStr;

            // Subcase 5.1: Chat message (type=4 or "chat")
            if (type == 4 || typeStr == "chat") {
                QString sender = obj.value("sender").toString();
                QString message = obj.value("message").toString();

                // Fallback for sender name (support multiple field names for compatibility)
                if (sender.isEmpty()) {
                    sender = obj.value("username").toString();
                    if (sender.isEmpty()) {
                        sender = obj.value("player_id").toString();
                    }
                }

                qDebug() << "Received chat message - Sender:" << sender << "Content:" << message;

                if (!message.isEmpty()) {
                    onChatMessageReceived(sender.isEmpty() ? "System" : sender, message);
                    return;
                }
            }

            // Subcase 5.2: Drawer word assignment (JSON format: "type":"drawer_word")
            if (obj.contains("type") && obj.value("type").toString() == "drawer_word") {
                QString word = obj.value("word").toString();
                addLog(QString("🎨 Drawer word: %1 - You are the drawer!").arg(word));
                // Save and display drawer word in UI
                m_currentWord = word;
                updateWordLabel(word);
                onRoundStarted(true); // Start game in drawer mode
                return;
            }

            // Subcase 5.3: Round control messages
            if (type == 30) { // Round start (JSON format)
                const QString drawerName = obj.value("drawer").toString();
                addLog(QString("Received round start message (JSON), drawer: %1").arg(drawerName));
                onDrawerAssigned(drawerName);
                return;
            } else if (type == 31) { // Round end (JSON format)
                qDebug() << "Received round end message (JSON format)";
                onRoundEnded();
                return;
            } else if (type == 32) { // Game end (JSON format)
                addLog("Received game end message (JSON format)");
                onGameEnded();
                return;
            } else if (type == 33) { // Drawer assignment (JSON format)
                const QString drawerName = obj.value("drawer").toString();
                addLog(QString("Received drawer assignment (JSON), drawer: %1").arg(drawerName));
                onDrawerAssigned(drawerName);
                return;
            } else if (type == 34) { // Correct guess (JSON format)
                QString guesserName = obj.value("guesser").toString();
                if (!guesserName.isEmpty()) {
                    addLog(QString("Received correct guess (JSON), player %1 guessed right").arg(guesserName));
                    onGuessCorrect(guesserName);
                } else {
                    addLog("Received correct guess (JSON)");
                    onGuessCorrect();
                }
                return;
            } else if (type == 35) { // Incorrect guess (JSON format)
                if (obj.contains("guesser")) {
                    QString guesserName = obj.value("guesser").toString();
                    addLog(QString("Received wrong guess (JSON), player %1 guessed wrong").arg(guesserName));
                    onGuessWrong(guesserName);
                }
                return;
            }

            // Subcase 6: Handle response_type based messages
            if (obj.contains("response_type")) {
                int responseType = obj.value("response_type").toInt();
                if (responseType == 21) { // Game end response
                    addLog("Received game end response (JSON)");
                    onGameEnded();
                    return;
                } else if (responseType == 22) { // Round start response
                    addLog("Received round start response (JSON)");
                    // Wait for explicit drawer assignment from server
                    return;
                } else if (responseType == 5) { // Correct guess response
                    if (obj.value("status").toString() == "guess_correct") {
                        addLog("Received correct guess response (JSON)");

                        // Update player score in local database (only for guesser)
                        if (!m_isDrawer && m_dbHandler && m_dbHandler->isValid() && !m_playerName.isEmpty()) {
                            // +10 points, +1 correct guess, 0 wrong guesses
                            m_dbHandler->updatePlayerScore(m_playerName, 10, 1, 0);
                            qDebug() << "Correct guess! Updated score for:" << m_playerName;
                            updateLeaderboard(); // Refresh leaderboard UI
                        }

                        onGuessCorrect();
                    }
                    return;
                }
            }
            // Subcase 7: Handle remote drawing synchronization (type=3)
            if (type == 3) {
                const QString action = obj.value("action").toString();

                // Sync drawing actions from other players:
                // begin: Start a new stroke (color/width/eraser)
                // point: Add a point to the stroke (x/y coordinates)
                // end: End the current stroke
                // undo: Undo last stroke
                // clear: Clear entire canvas
                if (action == "begin") {
                    QString colorStr = obj.value("color").toString("#000000");
                    QColor c(colorStr);
                    int w = obj.value("width").toInt(2);
                    bool eraser = obj.value("eraser").toBool(false);
                    m_canvas->remoteStorkeBegin(c, w, eraser);
                } else if (action == "point") {
                    int x = obj.value("x").toInt(0);
                    int y = obj.value("y").toInt(0);
                    m_canvas->remoteStrokePoint(QPoint(x, y));
                } else if (action == "end") {
                    m_canvas->remoteStrokeEnd();
                } else if (action == "undo") {
                    m_canvas->remoteUndoLastStroke();
                } else if (action == "clear") {
                    m_canvas->remoteClearCanvas();
                }
                return;
            }

            // Subcase 8: Skip drawing response messages (response_type=3)
            if (obj.contains("response_type") && obj.value("response_type").toInt() == 3) {
                return; // Don't log drawing response messages (internal sync)
            }
        }

        // Part 3: Log non-drawing messages (filter out type=3 to avoid spam)
        if (!msg.contains("\"type\":3") && !msg.contains("\"response_type\":3")) {
            addLog(QString::fromUtf8("[Message] ") + msg);
        }
    });
}

void MainWindow::wireUiSignals()
{
    // 颜色按钮：弹出颜色对话框，设置画笔颜色，并关闭橡皮擦
    connect(ui->colorButton, &QPushButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(Qt::black, this, tr("选择画笔颜色"));
        if (c.isValid()) {
            m_canvas->setColor(c);
            ui->eraserButton->setChecked(false);
            m_canvas->setEraser(false);
        }
    });

    // 线宽滑条：修改当前画笔粗细
    connect(ui->widthSlider, &QSlider::valueChanged, m_canvas, &Canvas::setWidth);

    // 橡皮擦按钮：切换橡皮擦模式
    connect(ui->eraserButton, &QPushButton::toggled, this, [this](bool checked) {
        m_canvas->setEraser(checked);
    });

    // 撤回上一笔 - 只在画手身份且游戏进行中可用
    connect(ui->undoButton, &QPushButton::clicked, this, [this]() {
        if (m_isDrawer && m_canvas->isRoundActive()) {
            // 先发送撤回消息到网络
            QJsonObject obj{{"type", 3}, {"action", "undo"}};
            m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
            qDebug() << "发送撤回消息";
            
            // 然后本地撤回（这样画师的撤回会立即响应，不会等待网络延迟）
            m_canvas->undoLastStroke();
        }
    });

    // 清空画布 - 只在画手身份且游戏进行中可用
    connect(ui->clearButton, &QPushButton::clicked, this, [this]() {
        if (m_isDrawer && m_canvas->isRoundActive()) {
            // 先发送清空消息到网络
            QJsonObject obj{{"type", 3}, {"action", "clear"}};
            m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
            qDebug() << "发送清空画布消息";
            
            // 然后本地清空
            m_canvas->clearCanvas();
        }
    });

    // 连接/断开
    if (auto btn = findChild<QPushButton*>("btnConnect")) {
        connect(btn, &QPushButton::clicked, this, [this]() {
            const QString urlStr = findChild<QLineEdit*>("serverUrlEdit") ? findChild<QLineEdit*>("serverUrlEdit")->text() : QString();
            m_playerName = findChild<QLineEdit*>("playerNameEdit") ? findChild<QLineEdit*>("playerNameEdit")->text() : QString();
            if (urlStr.isEmpty() || m_playerName.isEmpty()) { 
                addLog(QString::fromUtf8("❌ 请输入服务器地址与玩家名")); 
                updateConnectionStatus(false, "请输入信息");
                return; 
            }
            updateConnectionStatus(false, "正在连接...");
            m_conn->connectTo(QUrl(urlStr));
        });
    }
    if (auto btn = findChild<QPushButton*>("btnDisconnect")) {
        connect(btn, &QPushButton::clicked, this, [this]() { 
            m_conn->disconnectFrom(); 
            m_isHost = false;
            updateHostButtons(false);
        });
    }

    // 创建房间 - 只有创建房间的人才是房主
    if (auto btn = findChild<QPushButton*>("btnCreateRoom")) {
        connect(btn, &QPushButton::clicked, this, [this]() {
            const QString roomName = findChild<QLineEdit*>("roomNameEdit") ? findChild<QLineEdit*>("roomNameEdit")->text() : QString();
            QJsonObject obj{{"type", 10}, {"room_name", roomName}};
            m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
        });
    }

    // 加入房间 - 显示房间列表
    if (auto btn = findChild<QPushButton*>("btnJoinRoom")) {
        btn->setText("刷新房间列表");
        connect(btn, &QPushButton::clicked, this, &MainWindow::refreshRoomList);
    }

    // 游戏控制（房主）- 只有房主能操作
    auto sendSimpleType = [this](int t){ 
        if (m_isHost) {
            qDebug() << "发送消息类型:" << t;
            QJsonObject o{{"type", t}}; 
            m_conn->sendText(QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact))); 
        } else {
            addLog("只有房主可以操作游戏控制");
        }
    };
    
    if (auto btn = findChild<QPushButton*>("btnGameStart")) {
        connect(btn, &QPushButton::clicked, this, [sendSimpleType](){ 
            sendSimpleType(20); 
        });
    }
    if (auto btn = findChild<QPushButton*>("btnGameEnd")) {
        connect(btn, &QPushButton::clicked, this, [sendSimpleType](){ 
            sendSimpleType(21); 
        });
    }
    if (auto btn = findChild<QPushButton*>("btnRoundStart")) {
        connect(btn, &QPushButton::clicked, this, [this, sendSimpleType](){ 
            addLog("点击了开始回合按钮");
            sendSimpleType(22); 
        });
    }

    // 聊天窗口按钮
    if (auto btn = findChild<QPushButton*>("btnSendChat")) {
        btn->setText("💬 聊天");
        connect(btn, &QPushButton::clicked, this, &MainWindow::showChatWindow);
    }
    
    // 猜词按钮
    if (auto btn = findChild<QPushButton*>("btnSendGuess")) {
        connect(btn, &QPushButton::clicked, this, [this]() {
            const QString guess = findChild<QLineEdit*>("guessEdit") ? findChild<QLineEdit*>("guessEdit")->text().trimmed() : QString();
            if (guess.isEmpty()) return;
            QJsonObject obj{{"type", 5}, {"guess", guess}};
            m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
            if (auto le = findChild<QLineEdit*>("guessEdit")) le->clear();
        });
    }
    
    // 排行榜按钮
    if (auto btn = findChild<QPushButton*>("btnLeaderboard")) {
        connect(btn, &QPushButton::clicked, this, &MainWindow::toggleLeaderboard);
    }
}

//brief Append a log message to the log widget with timestamp,param msg Log message content,
void MainWindow::addLog(const QString& msg)
{
    if (!m_log) return; // Guard clause: exit if log widget is uninitialized
    // Format: [HH:mm:ss] message (add timestamp for traceability)
    m_log->append(QStringLiteral("[%1] %2").arg(QTime::currentTime().toString("HH:mm:ss")).arg(msg));
}

QString MainWindow::colorToHex(const QColor& c) const
{
    return c.name(QColor::HexRgb); // Return RGB hex string (e.g., red = #FF0000)
}


void MainWindow::updateControlButtons(bool isDrawer)
{
    // Calculate button enabled state (drawer + active round = enabled)
    bool enabled = isDrawer && m_canvas->isRoundActive();

    // Update enabled state for all drawing tools
    ui->undoButton->setEnabled(enabled);
    ui->clearButton->setEnabled(enabled);
    ui->colorButton->setEnabled(enabled);
    ui->eraserButton->setEnabled(enabled);
    ui->widthSlider->setEnabled(enabled);

    // Update tooltips (guide user why buttons are disabled/enabled)
    if (!enabled) {
        ui->undoButton->setToolTip("Game not active or you are not the drawer");
        ui->clearButton->setToolTip("Game not active or you are not the drawer");
        ui->colorButton->setToolTip("Game not active or you are not the drawer");
        ui->eraserButton->setToolTip("Game not active or you are not the drawer");
        ui->widthSlider->setToolTip("Game not active or you are not the drawer");
    } else {
        ui->undoButton->setToolTip("Undo last stroke");
        ui->clearButton->setToolTip("Clear entire canvas");
        ui->colorButton->setToolTip("Select brush color");
        ui->eraserButton->setToolTip("Toggle eraser mode");
        ui->widthSlider->setToolTip("Adjust brush width");
    }
}

//update the status of host's buttons
void MainWindow::updateHostButtons(bool isHost)
{
    qDebug() << "Update host button state:" << isHost;

    // Update "Start Game" button (host-only)
    if (auto btn = findChild<QPushButton*>("btnGameStart")) {
        btn->setEnabled(isHost);
        btn->setToolTip(isHost ? "Start game" : "Only host can start game");
    }

    // Update "End Game" button (host-only)
    if (auto btn = findChild<QPushButton*>("btnGameEnd")) {
        btn->setEnabled(isHost);
        btn->setToolTip(isHost ? "End game" : "Only host can end game");
    }

    // Update "Start Round" button (host-only)
    if (auto btn = findChild<QPushButton*>("btnRoundStart")) {
        btn->setEnabled(isHost);
        btn->setToolTip(isHost ? "Start round" : "Only host can start round");
    }
}


void MainWindow::onRoundStarted(bool isDrawer)
{
    qDebug() << "=== MainWindow::onRoundStarted START ===";
    qDebug() << "New round identity: isDrawer =" << isDrawer;
    qDebug() << "Previous identity state: m_isDrawer =" << m_isDrawer;

    // Clear canvas for new round (ensure clean slate)
    m_canvas->clearCanvas();

    // Update identity state (track previous state for logging)
    bool wasDrawer = m_isDrawer;
    m_isDrawer = isDrawer;

    // Mark round as active (critical for button/ canvas state)
    m_roundActive = true;

    // Log identity change (for debugging)
    if (wasDrawer != isDrawer) {
        qDebug() << "*** Identity changed ***";
        qDebug() << "From" << (wasDrawer ? "drawer" : "guesser") << "to" << (isDrawer ? "drawer" : "guesser");
    } else {
        qDebug() << "*** Identity unchanged ***" << (isDrawer ? "drawer" : "guesser");
    }

    // Update canvas state for all players:
    // - roundActive: Enable canvas interaction (draw/observe)
    // - myTurn: All players are in "their turn" (observe or draw)
    // - drawer: Mark canvas as drawer mode (enable drawing tools)
    m_canvas->setRoundActive(true);
    m_canvas->setMyTurn(true);
    m_canvas->setDrawer(isDrawer);

    qDebug() << "Canvas state updated - roundActive: true, myTurn: true, drawer:" << isDrawer;

    // Update drawing control buttons (enable only for drawer)
    updateControlButtons(isDrawer);

    // Drawer-specific logic (no input dialog, background timer only)
    if (isDrawer) {
        qDebug() << "*** Current player is drawer ***";
        addLog("Round started - You are the drawer, start drawing!");

        // Start 60-second background timer (no dialog for drawer)
        m_gameTimer->startTimer(60);

        // Force canvas refresh to ensure drawing state is applied immediately
        m_canvas->update();
        qDebug() << "Drawer canvas forced refresh - drawing should be available now";
    }
    // Guesser-specific logic (show input dialog with countdown)
    else {
        qDebug() << "*** Current player is guesser ***";
        addLog("Round started - You are the guesser, observe the drawing and guess the word!");

        // Clear word label for guesser (only drawer can see target word)
        if (m_currentWord.isEmpty()) {
            clearWordLabel();
        }

        // Start 60-second timer with guess input dialog (pass this as parent widget for centering)
        // "WORD" placeholder (actual word is set by server message)
        m_gameTimer->startGuessGame(60, "WORD", this);
        qDebug() << "Guess dialog should be displayed";

        // Force canvas refresh to ensure observer state is applied
        m_canvas->update();
        qDebug() << "Guesser canvas forced refresh";
        qDebug() << "*** Guesser canvas state check ***";
        qDebug() << "  m_canvas->roundActive:" << m_roundActive;
        qDebug() << "  Canvas internal state will be printed in canDraw()";
    }

    qDebug() << "=== MainWindow::onRoundStarted END ===";
}



void MainWindow::onRoundEnded()
{
    qDebug() << "=== MainWindow::onRoundEnded START ===";

    // Guard clause: prevent duplicate round end processing
    if (!m_roundActive) {
        qDebug() << "Round already ended, ignoring duplicate call";
        return;
    }

    qDebug() << "Previous round identity: m_isDrawer =" << m_isDrawer;

    // Mark round as inactive (core state reset)
    m_roundActive = false;

    // Stop game timer (both drawer/guesser modes)
    m_gameTimer->stopTimer();

    // Clear canvas for next round
    m_canvas->clearCanvas();

    // Fully reset canvas state (critical for preventing unintended interaction)
    m_canvas->setRoundActive(false);
    m_canvas->setMyTurn(false);
    m_canvas->setDrawer(false);

    // Reset identity state - critical: clear old identity for next round
    m_isDrawer = false;

    // Clear drawer word (no longer valid for next round)
    m_currentWord.clear();
    clearWordLabel();

    // Disable all drawing control buttons
    updateControlButtons(false);

    // Update and display current player ranking (scoreboard)
    updateRanking();

    addLog("⏸ Round ended");
    qDebug() << "Round state reset: roundActive=false, drawer=false, m_isDrawer=false";
    qDebug() << "=== MainWindow::onRoundEnded END ===";
}

void MainWindow::onGameEnded()
{
    qDebug() << "MainWindow::onGameEnded";
    
    // 停止计时器
    m_gameTimer->stopTimer();
    
    // 完全禁用画布
    m_canvas->setRoundActive(false);
    m_canvas->setMyTurn(false);
    m_canvas->setDrawer(false);
    m_isDrawer = false;
    
    // 清空画布
    m_canvas->clearCanvas();
    
    // 清除画手词
    m_currentWord.clear();
    clearWordLabel();
    
    // 禁用所有控制按钮
    updateControlButtons(false);
    
    // 显示最终排名
    showFinalRanking();
    
    addLog("🏁 游戏结束");
}

// 猜对后的处理
void MainWindow::onGuessCorrect(const QString& guesserName)
{
    qDebug() << "=== MainWindow::onGuessCorrect 开始 ===";
    qDebug() << "玩家" << guesserName << "猜对了，回合应该结束";
    
    // 防止回合已结束时重复加分
    if (!m_roundActive) {
        qDebug() << "回合已结束，不再更新分数:" << guesserName;
        return;
    }
    
    // 停止计时器
    m_gameTimer->stopTimer();
    
    // 更新猜对玩家的分数
    if (!guesserName.isEmpty() && guesserName != "guess_correct" && !guesserName.contains("status")) {
        updatePlayerScore(guesserName, true);
    } else {
        qDebug() << "无效的玩家名，跳过成绩更新:" << guesserName;
    }
    
    // 猜对后直接结束回合，onRoundEnded 会显示日志
    onRoundEnded();
    
    qDebug() << "=== MainWindow::onGuessCorrect 结束 ===";
}

// 猜错后的处理
void MainWindow::onGuessWrong(const QString& guesserName)
{
    qDebug() << "=== MainWindow::onGuessWrong 开始 ===";
    qDebug() << "玩家" << guesserName << "猜错了";
    
    if (!guesserName.isEmpty()) {
        updatePlayerScore(guesserName, false);
    }
    
    qDebug() << "=== MainWindow::onGuessWrong 结束 ===";
}

void MainWindow::onDrawerAssigned(const QString &drawerId)
{
    qDebug() << "=== onDrawerAssigned 被调用 ===";
    qDebug() << "收到的 drawerId:" << drawerId;
    qDebug() << "我的 m_playerId:" << m_playerId;
    qDebug() << "我的 m_playerName:" << m_playerName;
    qDebug() << "上一轮身份: m_isDrawer =" << m_isDrawer;
    
    // 判断这一轮我是不是画手
    bool isMe = false;
    
    // 方式1: 精确匹配 player_id（最可靠）
    if (!m_playerId.isEmpty() && drawerId == m_playerId) {
        isMe = true;
        qDebug() << "身份判断: player_id 精确匹配 → 我是画手";
    }
    // 方式2: 精确匹配，忽略空格
    else if (!m_playerId.isEmpty() && drawerId.trimmed() == m_playerId.trimmed()) {
        isMe = true;
        qDebug() << "身份判断: player_id 匹配(去除空格) → 我是画手";
    }
    // 方式3: 尝试用名字匹配
    else if (!m_playerName.isEmpty() && drawerId == m_playerName) {
        isMe = true;
        qDebug() << "身份判断: playerName 匹配 → 我是画手";
    }
    // 如果都不匹配，说明不是画手
    else {
        isMe = false;
        qDebug() << "身份判断: ID 不匹配 → 我是猜词者";
        qDebug() << "  drawerId ='" << drawerId << "' (长度:" << drawerId.length() << ")";
        qDebug() << "  m_playerId ='" << m_playerId << "' (长度:" << m_playerId.length() << ")";
    }
    
    qDebug() << "===========================================";
    qDebug() << "最终判断: isMe =" << isMe;
    qDebug() << "上一轮: m_isDrawer =" << m_isDrawer;
    qDebug() << "这一轮: isMe =" << isMe;
    
    if (m_isDrawer && !isMe) {
        qDebug() << "*** 身份切换: 画手 → 猜词者 ***";
        addLog("身份切换：你从画手变成了猜词者");
    } else if (!m_isDrawer && isMe) {
        qDebug() << "*** 身份切换: 猜词者 → 画手 ***";
        addLog("身份切换：你从猜词者变成了画手");
    } else if (isMe) {
        qDebug() << "*** 身份保持: 画手 → 画手 ***";
    } else {
        qDebug() << "*** 身份保持: 猜词者 → 猜词者 ***";
    }
    qDebug() << "===========================================";
    
    if (isMe) {
        // 画手：等待接收 drawer_word 后才启动
        addLog("你被选为画手！等待接收词语...");
        qDebug() << "画手不在此处启动，等待接收 drawer_word 消息";
        // 实际启动会在收到 drawer_word 消息时调用 onRoundStarted(true)
    } else {
        // 猜词者：立即启动猜词模式
        addLog(QString("画手是: %1，你是猜词者").arg(drawerId));
        qDebug() << "猜词者立即启动猜词模式";
        onRoundStarted(false); // 启动猜词模式
    }
    
    qDebug() << "=== onDrawerAssigned 结束 ===";
}

// ============================ 数据库集成函数 ============================

void MainWindow::updatePlayerScore(const QString& playerName, bool isCorrect)
{
    if (!m_dbHandler || playerName.isEmpty()) {
        qWarning() << "无法更新玩家分数 - 数据库未初始化或玩家名为空";
        return;
    }
    
    if (!m_dbHandler->isValid()) {
        qWarning() << "数据库无效，无法更新分数";
        return;
    }
    
    if (isCorrect) {
        m_dbHandler->updatePlayerScore(playerName, 10, 1, 0);
        addLog(QString("✓ %1 猜对 +10分").arg(playerName));
    } else {
        m_dbHandler->updatePlayerScore(playerName, 0, 0, 1);
        addLog(QString("✗ %1 猜错").arg(playerName));
    }
    
    qDebug() << "玩家" << playerName << (isCorrect ? "猜对" : "猜错") << "，分数已更新";
    
    // 更新排行榜显示
    updateLeaderboard();
}

void MainWindow::updateRanking()
{
    if (!m_dbHandler || !m_dbHandler->isValid()) {
        qWarning() << "数据库未初始化，无法获取排名";
        return;
    }
    
    QList<PlayerScoreInfo> rankings = m_dbHandler->getCurrentRanking();
    
    if (!rankings.isEmpty()) {
        addLog("📊 排名:");
        for (const PlayerScoreInfo& score : rankings) {
            int displayRank = (score.rank <= 0) ? 1 : score.rank;
            QString rankIcon = (displayRank == 1) ? "🥇" : (displayRank == 2) ? "🥈" : (displayRank == 3) ? "🥉" : "  ";
            QString logText = QString("%1 %2. %3: %4分 (✓%5 ✗%6)")
                .arg(rankIcon)
                .arg(displayRank)
                .arg(score.playerID)
                .arg(score.score)
                .arg(score.correctCount)
                .arg(score.wrongCount);
            addLog(logText);
        }
    }
}

void MainWindow::showFinalRanking()
{
    if (!m_dbHandler || !m_dbHandler->isValid()) return;
    
    QList<PlayerScoreInfo> rankings = m_dbHandler->getCurrentRanking();
    
    if (rankings.isEmpty()) {
        addLog("游戏结束，最终排名为空");
        return;
    }
    
    addLog("🏆 最终排名:");
    for (const PlayerScoreInfo& score : rankings) {
        int displayRank = (score.rank <= 0) ? 1 : score.rank;
        QString rankIcon = (displayRank == 1) ? "🥇" : (displayRank == 2) ? "🥈" : (displayRank == 3) ? "🥉" : "  ";
        QString logText = QString("%1 %2. %3: %4分 (✓%5 ✗%6)")
            .arg(rankIcon)
            .arg(displayRank)
            .arg(score.playerID)
            .arg(score.score)
            .arg(score.correctCount)
            .arg(score.wrongCount);
        addLog(logText);
    }
}

// ========== 云母效果相关实现 ==========

void MainWindow::enableMicaEffect()
{
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());
    
    // 设置窗口属性以支持云母效果
    // 1. 使用浅色模式（FALSE = 浅色，TRUE = 深色）
    BOOL useDarkMode = FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
    
    // 2. 设置系统背景类型为 Mica
    int backdropType = DWMSBT_MAINWINDOW;  // Mica 效果
    HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
    
    if (SUCCEEDED(hr)) {
        m_micaEnabled = true;
        qDebug() << "云母效果已启用（浅色模式）";
        
        // 不设置 WA_TranslucentBackground，保留系统标题栏和按钮
        // 设置窗口背景为半透明，让云母效果透过
        setStyleSheet(R"(
            QMainWindow {
                background-color: rgba(243, 243, 243, 230);
            }
            QWidget#centralwidget {
                background-color: rgba(255, 255, 255, 200);
            }
        )");
    } else {
        qDebug() << "启用云母效果失败，可能系统不支持（需要 Windows 11）";
        m_micaEnabled = false;
    }
#else
    qDebug() << "云母效果仅在 Windows 11 上支持";
#endif
}

void MainWindow::disableMicaEffect()
{
#ifdef Q_OS_WIN
    if (m_micaEnabled) {
        HWND hwnd = reinterpret_cast<HWND>(winId());
        
        // 禁用系统背景效果
        int backdropType = 1;  // DWMSBT_NONE
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
        
        // 恢复窗口背景
        setStyleSheet("");
        
        m_micaEnabled = false;
        qDebug() << "云母效果已禁用";
    }
#endif
}

#ifdef Q_OS_WIN
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    // 不处理 WM_NCCALCSIZE，保留系统标题栏和按钮
    return QMainWindow::nativeEvent(eventType, message, result);
}
#endif

// 处理窗口大小变化
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    
    // 调整画布区域大小（左侧占用大部分空间）
    int rightPanelWidth = 200;
    if (ui->canvasWidget) {
        ui->canvasWidget->setGeometry(-61, -51, width() - rightPanelWidth + 61, height() + 51);
        if (m_canvas) {
            m_canvas->setGeometry(ui->canvasWidget->rect());
        }
    }
    
    // 调整右侧控制面板位置
    if (auto widget = findChild<QWidget*>("widget")) {
        widget->setGeometry(width() - rightPanelWidth + 20, 20, 180, height() - 40);
    }
    
    // 调整日志窗口位置和大小
    if (m_log) {
        m_log->setGeometry(0, height() - 140, width() - rightPanelWidth, 120);
    }
    
    // 调整排行榜位置（居中显示）
    if (m_leaderboardTable) {
        int tableWidth = 500;
        int tableHeight = 400;
        int tableX = (width() - tableWidth) / 2;
        int tableY = (height() - tableHeight) / 2;
        
        m_leaderboardTable->setGeometry(
            tableX,
            tableY,
            tableWidth,
            tableHeight
        );
    }
    
    // 连接状态标签显示在右上角
    if (m_connectionStatusLabel) {
        m_connectionStatusLabel->setGeometry(
            width() - 160,  // 右侧留 10 像素边距
            10,              // 顶部留 10 像素边距
            70,
            20
        );
    }
}

// 更新画手词显示
void MainWindow::updateWordLabel(const QString &word)
{
    if (auto label = findChild<QLabel*>("wordLabel")) {
        label->setText(QString("画手词: %1").arg(word));
        qDebug() << "更新画手词显示:" << word;
    }
}

// 清除画手词显示
void MainWindow::clearWordLabel()
{
    if (auto label = findChild<QLabel*>("wordLabel")) {
        label->setText("画手词: ---");
        qDebug() << "清除画手词显示";
    }
}

// 更新排行榜
void MainWindow::updateLeaderboard()
{
    if (!m_dbHandler || !m_dbHandler->isValid()) {
        qWarning() << "数据库未初始化";
        return;
    }
    
    QList<PlayerScoreInfo> ranking = m_dbHandler->getCurrentRanking();
    
    m_leaderboardTable->setRowCount(ranking.size());
    
    for (int i = 0; i < ranking.size(); ++i) {
        const PlayerScoreInfo& info = ranking[i];
        
        m_leaderboardTable->setItem(i, 0, new QTableWidgetItem(QString::number(info.rank)));
        m_leaderboardTable->setItem(i, 1, new QTableWidgetItem(info.playerID));
        m_leaderboardTable->setItem(i, 2, new QTableWidgetItem(QString::number(info.score)));
        m_leaderboardTable->setItem(i, 3, new QTableWidgetItem(QString::number(info.correctCount)));
        
        // 高亮当前玩家（使用用户名）
        if (info.playerID == m_playerName) {
            for (int col = 0; col < 4; ++col) {
                m_leaderboardTable->item(i, col)->setBackground(QColor(255, 235, 59, 100));
            }
        }
        
        // 前三名使用不同颜色
        if (info.rank == 1) {
            m_leaderboardTable->item(i, 0)->setForeground(QColor(255, 215, 0)); // 金色
        } else if (info.rank == 2) {
            m_leaderboardTable->item(i, 0)->setForeground(QColor(192, 192, 192)); // 银色
        } else if (info.rank == 3) {
            m_leaderboardTable->item(i, 0)->setForeground(QColor(205, 127, 50)); // 铜色
        }
    }
    
    m_leaderboardTable->resizeColumnsToContents();
    qDebug() << "排行榜已更新，共" << ranking.size() << "名玩家";
}

// 显示/隐藏排行榜
void MainWindow::toggleLeaderboard()
{
    m_leaderboardVisible = !m_leaderboardVisible;
    
    if (m_leaderboardVisible) {
        updateLeaderboard();
        m_leaderboardTable->show();
        m_leaderboardTable->raise();
        addLog("排行榜已显示");
    } else {
        m_leaderboardTable->hide();
        addLog("排行榜已隐藏");
    }
}

// 显示聊天窗口
void MainWindow::showChatWindow()
{
    if (m_chatWindow) {
        m_chatWindow->show();
        m_chatWindow->raise();
        m_chatWindow->activateWindow();
    }
}

// 处理聊天消息接收
void MainWindow::onChatMessageReceived(const QString& sender, const QString& message)
{
    qDebug() << "✅ 处理聊天消息 - 发送者:" << sender << "消息:" << message;
    
    // 在主日志中显示简短提示
    addLog(QString("💬 %1: %2").arg(sender).arg(message));
    
    // 在聊天窗口中显示完整消息
    if (m_chatWindow) {
        m_chatWindow->addMessage(QString("%1: %2").arg(sender).arg(message));
    } else {
        qWarning() << "聊天窗口未初始化！";
    }
}

// 发送聊天消息
void MainWindow::onSendChatMessage(const QString& message)
{
    if (message.isEmpty() || !m_conn) {
        qWarning() << "无法发送消息: 消息为空或未连接";
        return;
    }
    
    QJsonObject obj{{"type", 4}, {"message", message}};
    QString jsonStr = QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    
    qDebug() << "📤 发送聊天消息:" << jsonStr;
    m_conn->sendText(jsonStr);
    
    addLog(QString("💬 已发送: %1").arg(message));
}

// 更新连接状态显示
void MainWindow::updateConnectionStatus(bool connected, const QString& message)
{
    if (!m_connectionStatusLabel) return;
    
    QString statusText;
    QString styleSheet;
    
    if (connected) {
        statusText = QString("✅ %1").arg(message.isEmpty() ? "已连接" : message);
        styleSheet = R"(
            QLabel {
                background-color: #4CAF50;
                color: white;
                padding: 4px 10px;
                border-radius: 4px;
                font-weight: bold;
                font-size: 11px;
            }
        )";
    } else {
        if (message == "正在连接...") {
            statusText = QString("🔄 %1").arg(message);
            styleSheet = R"(
                QLabel {
                    background-color: #FF9800;
                    color: white;
                    padding: 4px 10px;
                    border-radius: 4px;
                    font-weight: bold;
                    font-size: 11px;
                }
            )";
        } else {
            statusText = QString("❌ %1").arg(message.isEmpty() ? "未连接" : message);
            styleSheet = R"(
                QLabel {
                    background-color: #F44336;
                    color: white;
                    padding: 4px 10px;
                    border-radius: 4px;
                    font-weight: bold;
                    font-size: 11px;
                }
            )";
        }
    }
    
    m_connectionStatusLabel->setText(statusText);
    m_connectionStatusLabel->setStyleSheet(styleSheet);
    m_connectionStatusLabel->adjustSize();
}

// 更新房间信息显示
void MainWindow::updateRoomInfoDisplay(const QString& roomId, const QString& roomName)
{
    auto label = findChild<QLabel*>("roomStatusLabel");
    if (!label) return;
    
    if (roomId.isEmpty()) {
        label->setText("未加入房间");
    } else {
        QString displayText;
        if (!roomName.isEmpty()) {
            displayText = QString("🏠 %1").arg(roomName);
        } else {
            displayText = QString("🏠 %1").arg(roomId);
        }
        label->setText(displayText);
    }
}

// 刷新房间列表
void MainWindow::refreshRoomList()
{
    if (!m_conn || m_playerId.isEmpty()) {
        addLog("请先连接服务器");
        return;
    }
    
    addLog("🔄 获取房间列表中...");
    QJsonObject obj{{"type", 13}};
    m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

// 更新可用房间列表
void MainWindow::updateAvailableRooms(const QJsonArray& rooms)
{
    m_availableRooms.clear();
    
    // 解析房间信息（过滤无人房间）
    for (const QJsonValue& roomVal : rooms) {
        QJsonObject roomObj = roomVal.toObject();
        RoomInfo info;
        info.roomId = roomObj.value("room_id").toString();
        info.roomName = roomObj.value("room_name").toString();
        info.playerCount = roomObj.value("player_count").toInt(1);
        
        // 只添加有玩家的房间
        if (info.playerCount > 0) {
            m_availableRooms.append(info);
        }
    }
    
    // 清空旧的UI
    if (m_roomListWidget->layout()) {
        QLayout* oldLayout = m_roomListWidget->layout();
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }
    
    // 创建新的布局
    QVBoxLayout* mainLayout = new QVBoxLayout(m_roomListWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);
    
    // 标题
    QLabel* title = new QLabel("📋 可用房间列表", m_roomListWidget);
    title->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(title);
    
    // 滚动区域
    QScrollArea* scrollArea = new QScrollArea(m_roomListWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    
    QWidget* scrollWidget = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setSpacing(4);
    
    if (m_availableRooms.isEmpty()) {
        QLabel* emptyLabel = new QLabel("暂无可用房间，请创建新房间");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: #6c757d; padding: 20px;");
        scrollLayout->addWidget(emptyLabel);
    } else {
        for (const RoomInfo& info : m_availableRooms) {
            // 创建房间项
            QWidget* roomItem = new QWidget();
            roomItem->setCursor(Qt::PointingHandCursor);
            roomItem->setStyleSheet(R"(
                QWidget {
                    background: #f8f9fa;
                    border-left: 3px solid #3498db;
                    border-radius: 4px;
                    padding: 6px;
                }
                QWidget:hover {
                    background: #e3f2fd;
                }
            )");
            
            QHBoxLayout* itemLayout = new QHBoxLayout(roomItem);
            itemLayout->setContentsMargins(6, 6, 6, 6);
            
            // 房间信息
            QVBoxLayout* infoLayout = new QVBoxLayout();
            QLabel* nameLabel = new QLabel(info.roomName);
            nameLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
            QLabel* idLabel = new QLabel(QString("ID: %1").arg(info.roomId));
            idLabel->setStyleSheet("font-size: 10px; color: #6c757d;");
            infoLayout->addWidget(nameLabel);
            infoLayout->addWidget(idLabel);
            
            itemLayout->addLayout(infoLayout, 1);
            
            // 玩家数量
            QLabel* countLabel = new QLabel(QString("%1/6 人").arg(info.playerCount));
            countLabel->setStyleSheet(R"(
                background: #3498db;
                color: white;
                padding: 2px 8px;
                border-radius: 10px;
                font-size: 10px;
            )");
            itemLayout->addWidget(countLabel);
            
            // 连接点击事件
            roomItem->installEventFilter(this);
            roomItem->setProperty("roomId", info.roomId);
            
            scrollLayout->addWidget(roomItem);
        }
    }
    
    scrollWidget->setLayout(scrollLayout);
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
    
    // 关闭按钮
    QPushButton* closeBtn = new QPushButton("关闭", m_roomListWidget);
    closeBtn->setStyleSheet(R"(
        QPushButton {
            background: #e74c3c;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #c0392b;
        }
    )");
    connect(closeBtn, &QPushButton::clicked, this, [this]() {
        m_roomListWidget->hide();
    });
    mainLayout->addWidget(closeBtn);
    
    m_roomListWidget->setLayout(mainLayout);
    m_roomListWidget->show();
    m_roomListWidget->raise();
}

// 通过房间ID加入房间
void MainWindow::joinRoomById(const QString& roomId)
{
    if (roomId.isEmpty()) return;
    
    addLog(QString("🔍 加入房间: %1").arg(roomId));
    QJsonObject obj{{"type", 11}, {"room_id", roomId}};
    m_conn->sendText(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

// 事件过滤器 - 处理房间项点击
bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget && widget->property("roomId").isValid()) {
            QString roomId = widget->property("roomId").toString();
            joinRoomById(roomId);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

