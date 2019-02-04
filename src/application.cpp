#include "application.h"
#include <QTextCodec>
#include <QDebug>
#include <QMessageBox>

Application::Application(int &argc, char **argv) : QApplication(argc, argv) {
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    setOrganizationName("Konserw  Software");
    //QCoreApplication::setOrganizationDomain("koferta.no-ip.biz");
    setApplicationName("kOferta");
}

bool Application::notify(QObject *receiver, QEvent *event) {
    try {
        return QApplication::notify(receiver, event);
    } catch (const std::exception & e) {
        qCritical("%s exception has been thrown", e.what());
    } catch (...) {
        qCritical("Unknown Exception has been thrown");
    }
    QMessageBox::critical(nullptr,
                          tr("Niespodziewany błąd!"),
                          tr("Wystąpił niespodziewany błąd!\nNastąpi zamknięcie aplikacji.\nProszę o zgłoszenie tego błędu wraz z aktualnym plikiem .log na stronie: https://github.com/konserw/koferta/issues")
                          );
    exit(1);
    return false;
}
