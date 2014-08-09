#include "TermsChooserDialog.h"
#include "ui_TermsChooserDialog.h"


TermsChooserDialog::TermsChooserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TermsChooserDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Terms Chooser");
    m_model = nullptr;

    connect(ui->listView, SIGNAL(activated(QModelIndex)), this, SLOT(selectionChanged(QModelIndex)));
}

TermsChooserDialog::TermsChooserDialog(QWidget *parent, TermModel *model, QString title) :
    QDialog(parent),
    ui(new Ui::TermsChooserDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);

    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectionChanged(QModelIndex)));

    setDataModel(model);
}

TermsChooserDialog::~TermsChooserDialog()
{
    delete ui;
    delete m_model;
}

TermItem TermsChooserDialog::choosenTerm()
{
    return *static_cast<TermItem*>(ui->listView->currentIndex().internalPointer());
}

void TermsChooserDialog::selectionChanged(QModelIndex idx)
{
    ui->plainTextEdit->setPlainText(idx.sibling(idx.row(), 2).data().toString());
}

void TermsChooserDialog::setDataModel(TermModel* model)
{
    m_model = model;
    ui->listView->setModel(m_model);
    ui->listView->setModelColumn(1);
}
