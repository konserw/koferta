#ifndef EDYCJAKOMBO_H
#define EDYCJAKOMBO_H

#include <QDialog>
#include "Database.h"

namespace Ui {
class AddConditionDialog;
}

class AddConditionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddConditionDialog(Database::TermType type, QWidget *parent = 0);
    ~AddConditionDialog();

public slots:
    void ok();
    
private:
    Ui::AddConditionDialog *ui;

    Database::TermType m_type;

};

#endif // EDYCJAKOMBO_H
