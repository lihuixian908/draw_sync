#include "connect.h"
#include <QWebSocket>
#include <QNetworkProxy>
//initialize websocket
ConnectManager::ConnectManager(QObject* parent)
    : QObject(parent), m_socket(new QWebSocket())
{
    //Disable system network proxy
    m_socket->setProxy(QNetworkProxy::NoProxy);
    //bind formal signal with this class fuctions

    connect(m_socket, &QWebSocket::connected, this, &ConnectManager::onConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &ConnectManager::onDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &ConnectManager::onTextMessageReceived);
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    //be compatible with different version of qt
    connect(m_socket, &QWebSocket::errorOccurred, this, &ConnectManager::onError);
#else
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &ConnectManager::onError);
#endif
}

ConnectManager::~ConnectManager()
{
    if (m_socket)
    {
        //delete safely
        m_socket->deleteLater();
        //clear the pointer
        m_socket = nullptr;
    }
}

void ConnectManager::connectTo(const QUrl& url)
{
    if (!url.isValid())
    {
        //check if the url is valid
        QString errMsg = QString::fromUtf8("❌ URL 无效: ") + url.toString() + QString::fromUtf8(" (错误: ") + url.errorString() + QString::fromUtf8(")");
        emit log(errMsg);
        emit errorOccurred(errMsg);
        return;
    }

    if (url.scheme() != "ws" && url.scheme() != "wss")
    {
        //check if the 端口 is effective
        QString errMsg = QString::fromUtf8("❌ 协议错误: 仅支持 ws:// 或 wss://，当前: ") + url.scheme();
        emit log(errMsg);
        emit errorOccurred(errMsg);
        return;
    }

    if (url.port() < 0 || url.port() > 65535)
    {
        QString errMsg = QString::fromUtf8("❌ 端口无效: ") + QString::number(url.port());
        emit log(errMsg);
        emit errorOccurred(errMsg);
        return;
    }

    emit log(QString::fromUtf8("🔄 正在连接: ") + url.toString());
    m_socket->open(url);
}
//if in"is connected"situation，call it for break the connection
void ConnectManager::disconnectFrom()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        emit log(QString::fromUtf8("🔌 主动断开连接"));
        m_socket->close();
    }
}

//if in "is connected"situation,call it for send messages
void ConnectManager::sendText(const QString& text)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        m_socket->sendTextMessage(text);
        if (!text.contains("\"type\":3"))
        {
            emit log(QString::fromUtf8("📤 发送: ") + text);
        }
    }
    else
    {
        emit errorOccurred(QString::fromUtf8("❌ 未连接，无法发送"));
    }
}
//check
bool ConnectManager::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void ConnectManager::onConnected()
{
    emit log(QString::fromUtf8("✅ 连接成功: ") + m_socket->peerName() + QString::fromUtf8(":") + QString::number(m_socket->peerPort()));
    emit connected();
}

void ConnectManager::onDisconnected()
{
    emit log(QString::fromUtf8("🔴 连接已断开"));
    emit disconnected();
}

//slot function for receive message
void ConnectManager::onTextMessageReceived(const QString& message)
{

    if (!message.contains("\"response_type\":3") && !message.contains("\"type\":3"))
    {
        emit log(QString::fromUtf8("📥 收到: ") + message);
    }
    emit messageReceived(message);
}

//deal with error
void ConnectManager::onError()
{
    QString errMsg = QString::fromUtf8("❌ WebSocket 错误: ") + m_socket->errorString();
    emit log(errMsg);
    emit errorOccurred(errMsg);

}

