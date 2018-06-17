#include <QTest>
#include <cucumber-cpp/autodetect.hpp>
#include <CustomerSelection.h>
#include <QApplication>
#include <QSettings>
#include <cstdlib>
#include "Database.hpp"
#include "Offer.hpp"
#include "kofertaSteps.hpp"
#include "../../build/MainWindow/ui_CustomerSelection.h"
#include "CustomerSearch.hpp"
#include <QLineEdit>
#include <QPrinter>
#include <QTableView>
#include <QDebug>
#include <QObject>
#include <iostream>

static int argc = 0;
static QApplication app(argc, 0);
static int milliseconds = -1;
QString MainWindowCtx::databaseConnectionCert = "kOf_Admin.ppk";

void MainWindowCtx::openConnection()
{
    QSettings settings;
    settings.beginGroup("connection");
    settings.setValue("autoconnect", false);
    settings.setValue("testDB", true);
    settings.endGroup();

    /* Create the QEventLoop */
    QEventLoop pause;
    QObject::connect(Database::instance(), SIGNAL(connectionSuccess()), &pause, SLOT(quit()));
    QObject::connect(Database::instance(), SIGNAL(connectionFail()), &pause, SLOT(quit()));
    qDebug() << "Opening database connection using cert name:" << databaseConnectionCert;
    /* The code that will run during the QEventLoop */
    Database::instance()->setupDatabaseConnection(databaseConnectionCert, "", false);
    /* Execute the QEventLoop - it will quit when the above finished due to the connect() */
    pause.exec();
    qDebug() << "Database connection opened";
}

int millisecondsToWait() {
    if (milliseconds < 0)
    {
        char* envVariable = getenv("QT_STEP_DELAY");
        milliseconds = (0 != envVariable) ? atoi(envVariable) : 0;
    }
    return milliseconds;
}

std::istream& operator>> (std::istream& in, QString& val) { std::string s; in >> s; val = s.c_str(); return in; }
std::ostream& operator<< (std::ostream& out, const QString& val) { out << val.toLatin1().data(); return out; }

/* Initialization feature */

GIVEN("^I have just turned on the application$") {
    qWarning() << "first step";
   // MainWindowCtx::openConnection(); does not work!
    cucumber::ScenarioScope<MainWindowCtx> windowCtx;
    auto window = &(windowCtx.get()->window);

    window->move(0, 0);
    window->show();
    QTest::qWaitForWindowExposed(window, 10000);
    QTest::qWait(millisecondsToWait());
}

GIVEN("^I have created new offer$") {
    qWarning() << "second step";
    cucumber::ScenarioScope<MainWindowCtx> windowCtx;
    auto window = &(windowCtx.get()->window);

    qDebug() << "new offer"; //logger to be implemented - qInstallMessageHendler appears not to be working

    window->newOffer();
    QTest::qWait(millisecondsToWait());
}

GIVEN("^I have loaded new offer$") {
    cucumber::ScenarioScope<MainWindowCtx> windowCtx;
    auto window = &(windowCtx.get()->window);

    window->loadOfferFromDatabase("5000/2016");
    QTest::qWait(millisecondsToWait());
}

THEN("^the offer table should be disabled$") {
    cucumber::ScenarioScope<MainWindowCtx> windowCtx;
    auto window = &(windowCtx.get()->window);

    QCOMPARE(window->isUiInitialized(), false);
    QTest::qWait(millisecondsToWait());
}

THEN("^the offer table should be enabled$") {
    cucumber::ScenarioScope<MainWindowCtx> windowCtx;
    auto window = &(windowCtx.get()->window);

    QCOMPARE(window->isUiInitialized(), true);
    QTest::qWait(millisecondsToWait());
}

/* E2E feature */

GIVEN("^I have added (\\d+) x (\\d+) - (\\S*)$") {
    REGEX_PARAM(unsigned int, quantity);
    REGEX_PARAM(unsigned int, id);
    REGEX_PARAM(QString, code);

    cucumber::ScenarioScope<MainWindowCtx> windowCtx;
    Offer* offer = windowCtx->window.getCurrentOffer();

    offer->updateMerchandiseList(id, quantity);
    windowCtx.get()->items.append(code);

    QTest::qWait(millisecondsToWait());
}

GIVEN("^I have choosen customer (.*) - (.*)$") {
    REGEX_PARAM(QString, search);
    REGEX_PARAM(QString, customer);

    cucumber::ScenarioScope<MainWindowCtx> windowCtx;
    windowCtx->searchCustomer(search);
    windowCtx->customer = customer;

    QTest::qWait(millisecondsToWait());
}

GIVEN("^I have choosen some terms$") {
    cucumber::ScenarioScope<MainWindowCtx> windowCtx;
    Offer* offer = windowCtx->window.getCurrentOffer();
    //todo
}

THEN("^the offer printout should contain all of it$") {
    cucumber::ScenarioScope<MainWindowCtx> windowCtx;
    MainWindow* window = &(windowCtx.get()->window);
    auto print = window->getCurrentOffer()->document();

    QPrinter* printer = new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName("testOffer.pdf");
    window->getCurrentOffer()->print(printer);
    delete printer;

    foreach(const QString& item, windowCtx->items)
        Q_ASSERT(print.contains(item));
    Q_ASSERT(print.contains(windowCtx->customer));

    QCOMPARE(window->isUiInitialized(), true);
    QTest::qWait(millisecondsToWait());
}

void MainWindowCtx::searchCustomer(const QString &search)
{
    Offer* offer = window.getCurrentOffer();

    CustomerSelection* pop = new CustomerSelection(&window);
    QObject::connect(pop, &CustomerSelection::selectionChanged, offer, &Offer::setCustomer);
    //pop->exec();
    pop->show();
    QTest::qWaitForWindowExposed(pop, 10000);
    QTest::keyClicks(pop->ui->widget->lineEdit, search);
    auto table = pop->ui->widget->tableView;
    QTest::mouseClick(table->viewport(), Qt::LeftButton, NULL, QPoint(1, table->rowViewportPosition(0)));
    QTest::mouseClick(pop->ui->pushButton_exit, Qt::LeftButton);
}
