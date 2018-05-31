#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include <QSettings>
#include <QShowEvent>

namespace Ui {
class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();

protected:
    void showEvent(QShowEvent *event);

private:
    Ui::Options *ui;
    QSettings *settings;
    QString webSocketUrlSectionName;
    int webSocketUrlSectionIndex;

private slots:
    void currentIndexChanged(int index);
};

#endif // OPTIONS_H
