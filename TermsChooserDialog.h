#ifndef TERMSCHOOSERDIALOG_H
#define TERMSCHOOSERDIALOG_H

#include <QAbstractTableModel>
#include <QDialog>
#include "TermModel.h"
#include "TermItem.h"

namespace Ui {
class TermsChooserDialog;
}

class TermsChooserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TermsChooserDialog(QWidget *parent = 0);
    explicit TermsChooserDialog(QWidget *parent, TermModel* model, QString title = "Terms Chooser");
    ~TermsChooserDialog();

    TermItem choosenTerm();

public slots:
    void selectionChanged(QModelIndex);
    void setDataModel(TermModel *);

private:
    QAbstractTableModel* m_model;
    Ui::TermsChooserDialog *ui;
};

#endif // TERMSCHOOSERDIALOG_H
