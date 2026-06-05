#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();
    
    // 添加聊天消息
    void addMessage(const QString& message);
    
    // 清空聊天记录
    void clearMessages();

signals:
    // 发送聊天消息信号
    void sendChatMessage(const QString& message);

private slots:
    void onSendButtonClicked();
    void onReturnPressed();

private:
    void setupUI();
    
    QTextBrowser *m_chatDisplay;
    QLineEdit *m_inputEdit;
    QPushButton *m_sendButton;
    QPushButton *m_clearButton;
};

#endif // CHATWINDOW_H
