#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

class QIODevice;
class QTextStream;
class QFile;

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger* instance();
    static void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

signals:
    void logMsg(QString);
    
public slots:
    bool setFilePath(const QString& path);

protected:
    Logger();
    virtual ~Logger();

    static Logger* m_instance;
    QTextStream *m_out;
    QFile* m_log;
    bool m_pathSet;
};

#endif // LOGGER_H
