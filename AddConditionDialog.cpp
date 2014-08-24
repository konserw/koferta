#include "AddConditionDialog.h"
#include "ui_AddConditionDialog.h"
#include "Database.h"

AddConditionDialog::AddConditionDialog(const QString &typ, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddConditionDialog)
{
    ui->setupUi(this);

    m_typ = typ;

    this->setWindowTitle(tr("Nowa opcja w menu %1").arg(m_typ));
    ui->label_typ->setText(tr("Dodaj nową opcję dla pola %1").arg(m_typ));

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

    insert_combo(m_typ, sh, lo);
}
