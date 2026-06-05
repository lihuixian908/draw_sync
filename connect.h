#ifndef CONNECT_H
#define CONNECT_H
#include <QObject>
#include <QString>
#include <QUrl>
#include <QWebSocket>

QT_FORWARD_DECLARE_CLASS(QWebSocket)

// aim to connect with websocket
class ConnectManager : public QObject {
    Q_OBJECT
public:
    explicit ConnectManager(QObject* parent = nullptr);
    ~ConnectManager();

    void connectTo(const QUrl& url);
    //initialize after connecting successfully
    void disconnectFrom();
    void sendText(const QString& text);

    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString& message);
    //print out the error messages
    void messageReceived(const QString& text);
    void log(const QString& text);

private slots:
    void onConnected();
    //be called when connect,send signals
    void onDisconnected();
    //be called when break,send signals
    void onTextMessageReceived(const QString& message);
    void onError();

private:
    QWebSocket* m_socket;
};

#endif // CONNECT_H
