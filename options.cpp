#include "options.h"
#include "ui_options.h"

#include <QDebug>
#include <QVariant>
#include "messageevent.h"

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);

    webSocketUrlSectionIndex = -1;

    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Options::currentIndexChanged);
    ui->okBtn->setEnabled(false);

    connect(ui->noBtn, &QPushButton::clicked, [&]() {
        this->hide();
    });

    connect(ui->okBtn, &QPushButton::clicked, [&]() {
        QString newWebSocketUrlSectionName = ui->comboBox->itemData(webSocketUrlSectionIndex).toString();

        qDebug() << "last url key" << webSocketUrlSectionName;
        qDebug() << "select index" << webSocketUrlSectionIndex;
        qDebug() << "select url value" << ui->comboBox->itemText(webSocketUrlSectionIndex);
        qDebug() << "select url key" << newWebSocketUrlSectionName;

        if (newWebSocketUrlSectionName != webSocketUrlSectionName) {

            settings->setValue("common/url", newWebSocketUrlSectionName.mid(4));
            settings->sync();

            MessageEvent *msgEvent = new MessageEvent();
            msgEvent->setMessage(1, newWebSocketUrlSectionName);
            QApplication::postEvent(this->parent(), msgEvent);
        }

        this->hide();
    });
}

Options::~Options()
{
    delete ui;
}

void Options::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    qDebug() << "show";

    webSocketUrlSectionName = settings->value("common/url").toString().prepend("url/");

    qDebug() << webSocketUrlSectionName;
    qDebug() << settings->allKeys();

    ui->comboBox->clear();

    int i = 0;
    foreach (QString sectionName, settings->allKeys().filter("url/")) {
        qDebug() << sectionName << "->" << settings->value(sectionName).toString();
        if (sectionName == webSocketUrlSectionName) {
            webSocketUrlSectionIndex = i;
            ui->info->setText(settings->value(sectionName).toString());
        }
        ui->comboBox->addItem(settings->value(sectionName).toString(), QVariant(sectionName));
        i++;
    }

    ui->comboBox->setCurrentIndex(webSocketUrlSectionIndex);
}

void Options::currentIndexChanged(int index)
{
    qDebug() << "select index" << index << "last index" << webSocketUrlSectionIndex;
    if (index == webSocketUrlSectionIndex) {
        ui->okBtn->setEnabled(false);
        return;
    }

    ui->info->setText(ui->comboBox->itemText(index));
    webSocketUrlSectionIndex = index;

    QString selectWebSocketUrlSectionName = ui->comboBox->itemData(index).toString();
    if (webSocketUrlSectionName == selectWebSocketUrlSectionName) {
        ui->okBtn->setEnabled(false);
        return;
    }

    ui->okBtn->setEnabled(true);
}
