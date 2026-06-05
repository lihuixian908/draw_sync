#ifndef FANCYMAINWINDOW_H
#define FANCYMAINWINDOW_H

#include "src/Fancy/StackWindow.h"
#include <QWidget>
#include <QTime>

class Canvas;
class ConnectManager;
class QTextBrowser;
class GameTimer;
class QPushButton;
class QSlider;

namespace fancy {
    class PushButton;
    class SidebarButton;
}

class FancyMainWindow : public fancy::StackWindow
{
    Q_OBJECT

public:
    explicit FancyMainWindow(QWidget *parent = nullptr);
    ~FancyMainWindow();

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
    void onGuessCorrect();

private:
    void setupUI();
    void setupSidebar();
    void updateControlButtons(bool isDrawer);
    void updateHostButtons(bool isHost);

private:
    // UI 组件
    QWidget *m_canvasPage;
    QWidget *m_settingsPage;
    Canvas *m_canvas;
    ConnectManager *m_conn;
    QTextBrowser *m_log;
    GameTimer *m_gameTimer;
    
    // 控制按钮
    fancy::PushButton *m_colorButton;
    fancy::PushButton *m_eraserButton;
    fancy::PushButton *m_clearButton;
    QSlider *m_penWidthSlider;
    
    // 侧边栏按钮
    fancy::SidebarButton *m_homeBtn;
    fancy::SidebarButton *m_settingsBtn;
    
    // 游戏状态
    QString m_playerName;
    QString m_playerId;
    QString m_currentRoom;
    bool m_isDrawer = false;
    bool m_isHost = false;
    QTime m_roundStartTime;
};

#endif // FANCYMAINWINDOW_H
