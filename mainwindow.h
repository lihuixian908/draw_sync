#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class Canvas;
class ConnectManager;
class QTextBrowser;
class GameTimer;
class DBHandler;
class QTableWidget;
class ChatWindow;
class QLabel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // 云母效果相关方法
    void enableMicaEffect();
    void disableMicaEffect();

private slots:
    void addLog(const QString& msg);
    void setupConnections();
    void wireUiSignals();
    QString colorToHex(const QColor& c) const;

    // 回合控制
    void onRoundStarted(bool isDrawer);
    void onRoundEnded();
    void onDrawerAssigned(const QString &drawerName);
    void onGameEnded();
    void onGuessCorrect(const QString& guesserName = ""); // 猜对后的处理
    void onGuessWrong(const QString& guesserName); // 猜错后的处理
    
    // 数据库相关
    void updatePlayerScore(const QString& playerName, bool isCorrect);
    void updateRanking();
    void showFinalRanking();

private:
    // 更新控制按钮状态
    void updateControlButtons(bool isDrawer);
    // 更新房主控制按钮状态
    void updateHostButtons(bool isHost);
    // 更新画手词显示
    void updateWordLabel(const QString &word);
    // 清除画手词显示
    void clearWordLabel();
    // 更新排行榜
    void updateLeaderboard();
    // 显示/隐藏排行榜
    void toggleLeaderboard();
    // 显示聊天窗口
    void showChatWindow();
    // 处理聊天消息
    void onChatMessageReceived(const QString& sender, const QString& message);
    // 发送聊天消息
    void onSendChatMessage(const QString& message);
    // 更新连接状态显示
    void updateConnectionStatus(bool connected, const QString& message = "");
    // 更新房间信息显示
    void updateRoomInfoDisplay(const QString& roomId, const QString& roomName);
    // 房间列表相关
    void refreshRoomList();
    void updateAvailableRooms(const QJsonArray& rooms);
    void joinRoomById(const QString& roomId);
    
#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#endif

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Ui::MainWindow *ui;
    Canvas *m_canvas;
    ConnectManager *m_conn;
    QTextBrowser *m_log;
    GameTimer *m_gameTimer;
    DBHandler *m_dbHandler;
    QTableWidget *m_leaderboardTable;
    ChatWindow *m_chatWindow;
    QLabel *m_connectionStatusLabel;  // 连接状态标签
    QString m_playerName;
    QString m_playerId;
    QString m_currentRoom;
    bool m_isDrawer = false;
    bool m_isHost = false; // 是否是房主
    QTime m_roundStartTime; // 回合开始时间，用于过滤即时猜对消息
    bool m_micaEnabled = false; // 云母效果是否启用
    bool m_roundActive = false; // 回合是否活跃，防止重复结束
    QString m_currentWord; // 当前画手词
    bool m_leaderboardVisible = false; // 排行榜是否可见
    
    // 房间列表相关
    struct RoomInfo {
        QString roomId;
        QString roomName;
        int playerCount;
    };
    QList<RoomInfo> m_availableRooms; // 可用房间列表
    QWidget* m_roomListWidget = nullptr; // 房间列表容器
};

#endif // MAINWINDOW_H
