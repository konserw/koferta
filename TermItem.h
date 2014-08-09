#include <QString>

#ifndef TERMITEM_H
#define TERMITEM_H

class TermItem
{
public:
    TermItem();
    TermItem(int id, QString shortDesc, QString longDesc);

    int id() const;
    QString longDesc() const;
    QString shortDesc() const;

private:
    int m_id;
    QString m_shortDesc;
    QString m_longDesc;
};

#endif // TERMITEM_H
