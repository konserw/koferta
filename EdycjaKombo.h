#ifndef EDYCJAKOMBO_H
#define EDYCJAKOMBO_H

#include <QDialog>

namespace Ui {
class EdycjaKombo;
}

class EdycjaKombo : public QDialog
{
    Q_OBJECT
    
public:
    explicit EdycjaKombo(const QString& typ, QWidget *parent = 0);
    ~EdycjaKombo();

public slots:
    void ok();
    
private:
    Ui::EdycjaKombo *ui;

    QString m_typ;
};

#endif // EDYCJAKOMBO_H
