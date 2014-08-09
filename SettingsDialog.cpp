#include <QSettings>

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    readSettings();

    connect(this, &QDialog::accepted, this, &SettingsDialog::writeSettings);
    connect(ui->add_pushButton, &QPushButton::clicked, this, &SettingsDialog::addHost);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::writeSettings()
{
    QSettings settings;
    settings.setValue("settings set", true);
    settings.beginGroup("connection");
    settings.setValue("autoconnect", ui->autologin_checkBox->isChecked());
    settings.setValue("testDB", ui->checkBox_testDB->isChecked());
    settings.setValue("SSL enabled", ui->ssl_checkBox->isChecked());
    settings.setValue("hosts", hostList());
    settings.setValue("selected host", ui->ip_comboBox->currentText());
    settings.endGroup();
}

void SettingsDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup("connection");
    ui->autologin_checkBox->setChecked(settings.value("autoconnect", true).toBool());
    ui->checkBox_testDB->setChecked(settings.value("testDB", false).toBool());
    ui->ssl_checkBox->setChecked(settings.value("SSL enabled", true).toBool());
    ui->ip_comboBox->addItems(settings.value("hosts", "koferta.no-ip.biz").toStringList());
    ui->ip_comboBox->setCurrentText(settings.value("selected host", "koferta.no-ip.biz").toString());
    settings.endGroup();
}

void SettingsDialog::addHost()
{
    QString ip = ui->add_lineEdit->text();
    if(!ip.isEmpty())
    {
        ui->ip_comboBox->addItem(ip);
        ui->ip_comboBox->setCurrentText(ip);
        ui->add_lineEdit->clear();
    }
}

QStringList SettingsDialog::hostList()
{
    QStringList items;
    for(int i=0; i<ui->ip_comboBox->count(); ++i)
        items.append(ui->ip_comboBox->itemText(i));
    return items;
}
