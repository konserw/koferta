#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

protected slots:
    void writeSettings();
    void readSettings();
    void addHost();

private:
    QStringList hostList();

    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
