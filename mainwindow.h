#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSound>
#include <QSystemTrayIcon>
#include <QWebSocket>
#include <QSettings>
#include <QTimer>
#include <QDateTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void updateMessage(QString message);
    void start();

private:
    Ui::MainWindow *ui;
    QUrl webUrl;
    QIcon appRedIcon;
    QIcon appBlueIcon;
    QSound *bellForMessage;
    QSystemTrayIcon *trayIcon;
    QWebSocket *webSocket;
    QSettings *settings;

    QTimer webSocketPingTimer;
    QTimer messageTipTimer;
    QDateTime webSocketLastPongTime;

    bool gotAppIconPosition;
    bool connectedService;
    bool trayIconSwitched;

    QPixmap stateOnPixmap;
    QPixmap stateOffPixmap;

    void quitClear();
    void connectServer();
    void resetTrayIcon();

protected:
    bool event(QEvent *event);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void on_listWidget_customContextMenuRequested(const QPoint &pos);
    void deleteMenuSelected();
    void clearMenuSelected();

    void onWebSocketConnected();
    void onWebSocketDisconnected();
    void onWebSocketMessageReceived(const QString &message);
    void onWebSocketClose();
    void onWebSocketError(QAbstractSocket::SocketError error);
    void onWebSocketStateChanged(QAbstractSocket::SocketState state);
    void onWebSocketPong(quint64 elapsedTime, const QByteArray &payload);
    void onWebSocketTimerTimeout();
    void onMessageTipTimerTimeout();

};

#endif // MAINWINDOW_H
