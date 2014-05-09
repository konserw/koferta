#ifndef EDYCJAKOMBO_H
#define EDYCJAKOMBO_H

#include <QDialog>

namespace Ui {
class AddConditionDialog;
}

class AddConditionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddConditionDialog(const QString& typ, QWidget *parent = 0);
    ~AddConditionDialog();

public slots:
    void ok();
    
private:
    Ui::AddConditionDialog *ui;

    QString m_typ;
};

#endif // EDYCJAKOMBO_H
