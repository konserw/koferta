#include <QTest>
#include <cucumber-cpp/autodetect.hpp>
#include <QApplication>
#include <cstdlib>
#include "MainWindow.h"

static int argc = 0;
static QApplication app(argc, 0);
static int milliseconds = -1;

int millisecondsToWait() {
    if (milliseconds < 0)
    {
        char* envVariable = getenv("CALCQT_STEP_DELAY");
        milliseconds = (0 != envVariable) ? atoi(envVariable) : 0;
    }
    return milliseconds;
}

std::istream& operator>> (std::istream& in, QString& val) { std::string s; in >> s; val = s.c_str(); return in; }
std::ostream& operator<< (std::ostream& out, const QString& val) { out << val.toLatin1().data(); return out; }

GIVEN("^I just turned on the application$") {
    cucumber::ScenarioScope<MainWindow> window;
    window->move(0, 0);
    window->show();
    QTest::qWaitForWindowExposed(window.get(), 10000);
    QTest::qWait(millisecondsToWait());
}

THEN("^the offer table should be disabled$") {
    cucumber::ScenarioScope<MainWindow> window;
    QCOMPARE(window->isOfferTableEnabled(), false);
    QTest::qWait(millisecondsToWait());
}
