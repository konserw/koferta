#include <QtDebug>
#include "DatabaseHelpers.hpp"

bool Transaction::opened = false;
bool Transaction::transactionsDisabled = false;

void Transaction::commit()
{
    if(opened == false)
    {
        qWarning() << "Tried to commit without opened transaction";
        return;
    }
    opened = false;
    if(QSqlDatabase::database().commit() == false)
    {
        QString error = QString("SQL transaction commit has failed!\n"
                                "\t* Error text: %1\n"
                                "\t* Rolling back transaction\n")
                .arg(QSqlDatabase::database().lastError().text());
        if(QSqlDatabase::database().rollback() == false)
        {
            error += QString("\t* SQL transaction rollback has failed!\n"
                             "\t* Error text: %1\n")
                    .arg(QSqlDatabase::database().lastError().text());
        }
        throw DatabaseException(error);
    }
}

void Transaction::disableTransactionFeature()
{
    transactionsDisabled = true;
}

void Transaction::open()
{
    if(transactionsDisabled)
        return;

    if(QSqlDatabase::database().transaction() == false)
    {
        QString error = QString("SQL transaction open has failed!\n"
                                "\t* Error text: %1\n")
                .arg(QSqlDatabase::database().lastError().text());
        throw DatabaseException(error);
    }
    opened = true;
}

void Transaction::handleError(const QSqlQuery &query)
{
    QString error = QString("SQL query has failed!\n"
                            "\t* Query: %1\n"
                            "\t* Query error text: %2\n"
                            "\t* Error text: %3\n")
            .arg(query.lastQuery())
            .arg(query.lastError().text())
            .arg(QSqlDatabase::database().lastError().text());
    if(opened)
    {
        error += "\t* Rolling back transaction\n";
        if(QSqlDatabase::database().rollback())
            error += "\t* Rollback successful\n";
        else
            error += QString("\t* SQL transaction rollback has failed!\n"
                             "\t* Error text: %1\n")
                    .arg(QSqlDatabase::database().lastError().text());
        opened = false;
    }
    throw DatabaseException(error);
}

QSqlQuery Transaction::run(const QString& queryText)
{
    QSqlQuery query;
    query.prepare(queryText);
    run(query);
    return query;
}

void Transaction::run(QSqlQuery &query)
{
    if(query.exec() == false)
        handleError(query);
}

void Transaction::runBatch(QSqlQuery &query)
{
    if(query.execBatch() == false)
        handleError(query);
}

QByteArray hmacSha1(QByteArray key, QByteArray baseString)
{
    int blockSize = 64; // HMAC-SHA-1 block size, defined in SHA-1 standard
    if (key.length() > blockSize) { // if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }
    QByteArray innerPadding(blockSize, char(0x36)); // initialize inner padding with char "6"
    QByteArray outerPadding(blockSize, char(0x5c)); // initialize outer padding with char "\"
    // ascii characters 0x36 ("6") and 0x5c ("\") are selected because they have large
    // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)

    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i); // XOR operation between every byte in key and innerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i); // XOR operation between every byte in key and outerpadding, of key length
    }

    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    return QCryptographicHash::hash(total, QCryptographicHash::Sha1);
}

QString saltPassword(const QString &salt, const QString &password)
{
    return hmacSha1(password.toUtf8(), salt.toUtf8()).toBase64();
}
