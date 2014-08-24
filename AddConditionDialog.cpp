#include "AddConditionDialog.h"
#include "ui_AddConditionDialog.h"
#include "Database.h"

AddConditionDialog::AddConditionDialog(Database::TermType type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddConditionDialog)
{
    ui->setupUi(this);

    m_type = type;
    QString typeTransaltion;
    switch(type)
    {
    case Database::termOffer:
        typeTransaltion = tr("\"Warunki Oferty\"");
        break;
    case Database::termPayment:
        typeTransaltion = tr("\"Warunki Termin Dostawy");
        break;
    case Database::termShipmentTime:
        typeTransaltion = tr("\"Termin Dostawy\"");
        break;
    case Database::termShipping:
        typeTransaltion = tr("\"Wraunki Dostawy\"");
        break;
    }

    this->setWindowTitle(tr("Nowa opcja w menu %1").arg(typeTransaltion));
    ui->label_typ->setText(tr("Dodaj nową opcję dla pola %1").arg(typeTransaltion));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
}

AddConditionDialog::~AddConditionDialog()
{
    delete ui;
}

void AddConditionDialog::ok()
{
    QString sh = ui->lineEdit_sort->text();
    QString lo = ui->textEdit->toPlainText();

    if(sh.isEmpty())
        return;

    Database::createTerms(m_type, sh, lo);
}
