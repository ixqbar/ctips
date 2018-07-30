#include <QDir>
#include <QUrl>
#include <QFont>
#include <QMenu>
#include <QDebug>
#include <QScreen>
#include <QByteArray>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>

#include "messageevent.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamehelper.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidget->setAcceptDrops(false);

    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);

    gotAppIconPosition = false;
    connectedService   = false;
    trayIconSwitched   = false;

    macAddress = GameHelper::getInstance().getMacAddress();
    qDebug() << "address:" << macAddress;

    bellForMessage = new QSound(":/Resources/message.wav");
    appRedIcon     = QIcon(":/Resources/red-icon.icns");
    appBlueIcon    = QIcon(":/Resources/blue-icon.icns");
    stateOnPixmap  = QPixmap::fromImage(QImage(":/Resources/light_on_16.png"));
    stateOffPixmap = QPixmap::fromImage(QImage(":/Resources/light_off_16.png"));

    ui->states->setPixmap(stateOffPixmap);

    settings   = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
    trayIcon   = new QSystemTrayIcon(this);

    optionsWin = NULL;
    clipboard = NULL;

    connect(ui->quitBtn, &QPushButton::clicked, [&]() {
        qApp->quit();
    });

    connect(ui->minBtn, &QPushButton::clicked, [&]() {
        if (optionsWin != NULL) {
            optionsWin->hide();
        }
        this->hide();
    });

    connect(ui->optBtn, &QPushButton::clicked, [&]() {
        if (optionsWin == NULL) {
            optionsWin = new Options(this);
        }

        if (optionsWin->isVisible() && optionsWin->isActiveWindow() == false) {
            optionsWin->raise();
        } else {
            optionsWin->show();
        }
    });

    QString webSocketUrlSectionName = "url/";
    webSocketUrlSectionName.append(settings->value("common/url").toString());

    qDebug() << "default websocket url section name:" << webSocketUrlSectionName;

    resetWebSocketUrl(webSocketUrlSectionName);
}

MainWindow::~MainWindow()
{
    quitClear();
}

void MainWindow::quitClear()
{
    if (webSocket != NULL) {
        webSocket->close();
        delete webSocket;
    }

    if (webSocketPingTimer.isActive()) {
        webSocketPingTimer.stop();
    }

    if (messageTipTimer.isActive()) {
        messageTipTimer.stop();
    }

    if (optionsWin != NULL) {
        delete optionsWin;
    }

    delete ui;
    delete bellForMessage;
    delete trayIcon;
    delete settings;
    delete optionsWin;
}

void MainWindow::resetWebSocketUrl(QString urlSectionName)
{
    webSocketUrl = QUrl(settings->value(urlSectionName).toString().append("&uuid=ctips:").append(macAddress));
    qDebug() << "reset websocket url" << webSocketUrl.toString();
}

void MainWindow::start()
{
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::messageClicked);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    trayIcon->setIcon(appBlueIcon);
    trayIcon->show();

    messageTipTimer.setInterval(500);
    connect(&messageTipTimer, &QTimer::timeout, this, &MainWindow::onMessageTipTimerTimeout);

    connectServer();
}

void MainWindow::messageClicked()
{
    qDebug() << "message notice clicked";

    resetTrayIcon();

    if (false == gotAppIconPosition) {
        if (qApp->screens().size() == 1) {
            this->move(trayIcon->geometry().x(), 22);
        } else {
            this->move(qApp->primaryScreen()->size().width() - 550, 22);
        }
    }

    this->show();
    this->raise();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    Q_UNUSED(reason);

    resetTrayIcon();

    QPoint cursorPoint = QCursor::pos();

    if (gotAppIconPosition
            && this->isVisible()
            && this->isActiveWindow() == false) {
        this->move(cursorPoint.x() - 16, cursorPoint.y() - 11);
        this->raise();
        return;
    }

    if (this->isVisible()) {
        this->hide();
        return;
    }

    gotAppIconPosition = true;

    this->move(cursorPoint.x() - 16, cursorPoint.y() - 11);
    this->show();
    this->raise();
}

void MainWindow::updateMessage(QString message)
{
    qDebug() << "receive notice message:" << message;

    QJsonParseError jsonErr;
    QJsonDocument responseDoc = QJsonDocument::fromJson(message.toUtf8(), &jsonErr);
    if (jsonErr.error != QJsonParseError::NoError
            || responseDoc.isEmpty()
            || responseDoc.isNull()) {
        qDebug() << "invalid notice message format:" << message;
        return;
    }

    QJsonObject responseObj = responseDoc.object();
    if (responseObj.contains("message") == false) {
        qDebug() << "invalid notice message format:" << message;
        return;
    }

    QString noticeTitle = responseObj.value("title").toString();
    QString noticeMessage = responseObj.value("message").toString();

    bellForMessage->play();

    QListWidgetItem *item = new QListWidgetItem();
    item->setText(noticeMessage);
    item->setSizeHint(QSize(470, 25));
    item->setToolTip(noticeMessage);
    item->setFont(QFont("PT mono", 14));

    ui->listWidget->insertItem(0, item);
    ui->listWidget->clearSelection();
    ui->infoLabel->setText(QDateTime::currentDateTime().toString("截止:yyyy-MM-dd hh:mm:ss"));

    if (ui->listWidget->count() > 1) {
        ui->listWidget->setAcceptDrops(true);
    }

    trayIcon->showMessage(
        noticeTitle.isEmpty() ? QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") : noticeTitle,
        noticeMessage,
        appBlueIcon,
        300000
    );
    trayIcon->setToolTip(message);

    if (messageTipTimer.isActive() == false) {
        messageTipTimer.start();
    }
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == MessageEvent::MessageEventType) {
        MessageEvent *msgEvent = static_cast<MessageEvent *>(event);

        qDebug() << "receive messageEvent type" << msgEvent->getMessageType() << "messageEvent message" << msgEvent->getMessage();

        switch (msgEvent->getMessageType()) {
        case 0:
            updateMessage(msgEvent->getMessage());
            break;
        case 1:
            resetWebSocketUrl(msgEvent->getMessage());
            webSocket->disconnected();
            webSocket->open(webSocketUrl);
            ui->listWidget->setAcceptDrops(false);
            ui->listWidget->clear();
            ui->infoLabel->setText("");
            resetTrayIcon();
            break;
        default:
            break;
        }

        return true;
    }

    return QMainWindow::event(event);
}

void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);

    if (ui->listWidget->count() == 0) {
        return;
    }

    QAction *copyMenu = new QAction(tr("copy"), this);
    QAction *deleteMenu = new QAction(tr("delete"), this);
    QAction *clearMenu = new QAction(tr("clear"), this);

    if (ui->listWidget->selectedItems().count() == 0) {
        copyMenu->setEnabled(false);
        deleteMenu->setEnabled(false);
    }

    QMenu* popMenu = new QMenu(this);
    popMenu->addAction(copyMenu);
    popMenu->addAction(deleteMenu);
    popMenu->addAction(clearMenu);
    connect(copyMenu, &QAction::triggered, this, &MainWindow::copyMenuSelected);
    connect(deleteMenu, &QAction::triggered, this, &MainWindow::deleteMenuSelected);
    connect(clearMenu, &QAction::triggered, this, &MainWindow::clearMenuSelected);
    popMenu->exec(QCursor::pos());
}


void MainWindow::copyMenuSelected()
{
    QList<QListWidgetItem*> itemList = ui->listWidget->selectedItems();
    if (itemList.count() == 0) {
        return;
    }

    if (clipboard == NULL) {
        clipboard = QApplication::clipboard();
    }

    clipboard->setText(itemList.first()->text());
}

void MainWindow::deleteMenuSelected()
{
    QList<QListWidgetItem*> itemList = ui->listWidget->selectedItems();
    if (itemList.count() == 0) {
        return;
    }

    QListWidgetItem *item;
    foreach (item, itemList) {
        ui->listWidget->removeItemWidget(item);
        delete item;
    }

    int num = ui->listWidget->count();
    if (num == 0) {
        ui->infoLabel->setText("");
        ui->listWidget->setAcceptDrops(false);
    } else if (num > 1) {
        ui->listWidget->setAcceptDrops(true);
    } else {
        ui->listWidget->setAcceptDrops(false);
    }
}

void MainWindow::clearMenuSelected()
{
    int result = QMessageBox::warning(this,
                                      "Warning",
                                      "Are you sure to clear logs ?",
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No);

    if (result != QMessageBox::Yes) {
        return;
    }

    ui->listWidget->setAcceptDrops(false);
    ui->listWidget->clear();
    ui->infoLabel->setText("");

    resetTrayIcon();
}

void MainWindow::connectServer()
{
    if (macAddress.length() == 0) {
        return;
    }

    QString origin;
    origin.append("rumbladeApp:").append(macAddress);

    webSocket = new QWebSocket(origin);
    connect(webSocket, &QWebSocket::connected, this, &MainWindow::onWebSocketConnected);
    connect(webSocket, &QWebSocket::disconnected, this, &MainWindow::onWebSocketDisconnected);
    connect(webSocket, &QWebSocket::textMessageReceived, this, &MainWindow::onWebSocketMessageReceived);
    connect(webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &MainWindow::onWebSocketError);
    connect(webSocket, &QWebSocket::stateChanged, this, &MainWindow::onWebSocketStateChanged);
    connect(webSocket, &QWebSocket::pong, this, &MainWindow::onWebSocketPong);

    webSocket->open(webSocketUrl);

    webSocketPingTimer.setInterval(15000);
    webSocketPingTimer.start();
    connect(&webSocketPingTimer, &QTimer::timeout, this, &MainWindow::onWebSocketTimerTimeout);
}

void MainWindow::onWebSocketConnected()
{
    qDebug() << "webSocket connected";

    ui->states->setPixmap(stateOnPixmap);

    connectedService = true;
    webSocketLastPongTime = QDateTime::currentDateTime();
}

void MainWindow::onWebSocketDisconnected()
{
    qDebug() << "webSocket disconnected";

    ui->states->setPixmap(stateOffPixmap);

    connectedService = false;
}

void MainWindow::onWebSocketMessageReceived(const QString &message)
{
    qDebug() << "webSocket message received:" << message;
    updateMessage(message);
}

void MainWindow::onWebSocketPong(quint64 elapsedTime, const QByteArray &payload)
{
    qDebug() << "websocket got pong:" << elapsedTime << QString(payload);

    webSocketLastPongTime = QDateTime::currentDateTime();
}

void MainWindow::onWebSocketClose()
{
    qDebug() << "to close websocket";
    webSocket->close();
}

void MainWindow::onWebSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << "websocket error:" << error;
}

void MainWindow::onWebSocketStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "websocket state changed:" << state;
}

void MainWindow::onWebSocketTimerTimeout()
{
    qDebug() << "websocket timer timeout";

    if (connectedService && webSocketLastPongTime.addSecs(30) > QDateTime::currentDateTime()) {
        webSocket->ping(QByteArray().append("PING"));
    } else {
        qDebug() << "websocket reconnect for timeout";
        webSocket->close();
        webSocket->open(webSocketUrl);
    }
}

void MainWindow::onMessageTipTimerTimeout()
{
    if (trayIconSwitched == false) {
        trayIconSwitched = true;
        trayIcon->setIcon(appRedIcon);
    } else {
        trayIconSwitched = false;
        trayIcon->setIcon(appBlueIcon);
    }
}

void MainWindow::resetTrayIcon()
{
    trayIconSwitched = false;

    trayIcon->setIcon(appBlueIcon);
    trayIcon->setToolTip("");

    messageTipTimer.stop();
}
