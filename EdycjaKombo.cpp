#include "EdycjaKombo.h"
#include "ui_EdycjaKombo.h"
#include "Database.h"

EdycjaKombo::EdycjaKombo(const QString &typ, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EdycjaKombo)
{
    ui->setupUi(this);

    m_typ = typ;

    this->setWindowTitle(tr("Nowa opcja w menu %1").arg(m_typ));

    ui->label_typ->setText(tr("Dodaj nową opcję dla pola %1").arg(m_typ));
    ui->label_short->setText(tr("Tekst widoczny w menu rozwijalnym:"));
    ui->label_long->setText(tr("Drukowany tekst:"));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
//    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

EdycjaKombo::~EdycjaKombo()
{
    delete ui;
}

void EdycjaKombo::ok()
{
    QString sh = ui->lineEdit_sort->text();
    QString lo = ui->textEdit->toPlainText();

    if(sh.isEmpty() || lo.isEmpty())
        return;

    insert_combo(m_typ, sh, lo);
}
