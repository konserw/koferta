#ifndef CLIENT_H
#define CLIENT_H

#include<QString>

class Client
{
public:
    Client(
        const QString& ShortName,
        const QString& FullName,
        const QString& Title,
        const QString& Name,
        const QString& Surname,
        const QString& Address
           );

    QString concatedName() const;
    QString getAddress() const;
    QString getShortName() const;
    QString getFullName() const;

private:
    QString shortName;
    QString fullName;
    QString title;
    QString name;
    QString surname;
    QString address;


};

#endif // CLIENT_H
