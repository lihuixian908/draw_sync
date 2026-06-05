#include "ChatWindow.h"
#include <QTime>
#include <QScrollBar>

ChatWindow::ChatWindow(QWidget *parent)
    : QWidget(parent)
    , m_chatDisplay(nullptr)
    , m_inputEdit(nullptr)
    , m_sendButton(nullptr)
    , m_clearButton(nullptr)
{
    setupUI();
    setWindowTitle("聊天窗口");
    resize(400, 500);
    
    // 设置窗口标志，使其成为独立窗口
    setWindowFlags(Qt::Window);
}

ChatWindow::~ChatWindow()
{
}

void ChatWindow::setupUI()
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // 聊天显示区域
    m_chatDisplay = new QTextBrowser(this);
    m_chatDisplay->setPlaceholderText("聊天消息将显示在这里...");
    m_chatDisplay->setStyleSheet(R"(
        QTextBrowser {
            background-color: rgba(255, 255, 255, 250);
            border: 2px solid #E0E0E0;
            border-radius: 8px;
            padding: 8px;
            font-size: 13px;
        }
    )");
    mainLayout->addWidget(m_chatDisplay, 1);
    
    // 输入区域布局
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(8);
    
    m_inputEdit = new QLineEdit(this);
    m_inputEdit->setPlaceholderText("输入聊天消息...");
    m_inputEdit->setStyleSheet(R"(
        QLineEdit {
            border: 2px solid #2196F3;
            border-radius: 6px;
            padding: 8px;
            font-size: 13px;
            background-color: white;
        }
        QLineEdit:focus {
            border: 2px solid #1976D2;
        }
    )");
    inputLayout->addWidget(m_inputEdit, 1);
    
    m_sendButton = new QPushButton("发送", this);
    m_sendButton->setStyleSheet(R"(
        QPushButton {
            background-color: #2196F3;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #1976D2;
        }
        QPushButton:pressed {
            background-color: #0D47A1;
        }
    )");
    inputLayout->addWidget(m_sendButton);
    
    m_clearButton = new QPushButton("清空", this);
    m_clearButton->setStyleSheet(R"(
        QPushButton {
            background-color: #FF9800;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-size: 13px;
        }
        QPushButton:hover {
            background-color: #F57C00;
        }
        QPushButton:pressed {
            background-color: #E65100;
        }
    )");
    inputLayout->addWidget(m_clearButton);
    
    mainLayout->addLayout(inputLayout);
    
    // 连接信号槽
    connect(m_sendButton, &QPushButton::clicked, this, &ChatWindow::onSendButtonClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &ChatWindow::clearMessages);
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &ChatWindow::onReturnPressed);
}

void ChatWindow::addMessage(const QString& message)
{
    if (!m_chatDisplay) return;
    
    // 添加时间戳
    QString timestamp = QTime::currentTime().toString("HH:mm:ss");
    QString formattedMessage = QString("[%1] %2").arg(timestamp).arg(message);
    
    m_chatDisplay->append(formattedMessage);
    
    // 自动滚动到底部
    QScrollBar *scrollBar = m_chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWindow::clearMessages()
{
    if (m_chatDisplay) {
        m_chatDisplay->clear();
        addMessage("聊天记录已清空");
    }
}

void ChatWindow::onSendButtonClicked()
{
    if (!m_inputEdit) return;
    
    QString message = m_inputEdit->text().trimmed();
    if (message.isEmpty()) return;
    
    // 发送消息
    emit sendChatMessage(message);
    
    // 在本地显示
    addMessage("我: " + message);
    
    // 清空输入框
    m_inputEdit->clear();
    m_inputEdit->setFocus();
}

void ChatWindow::onReturnPressed()
{
    onSendButtonClicked();
}
