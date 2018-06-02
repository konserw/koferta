set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools)
set(QT_LIBRARIES Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Network Qt5::PrintSupport)

set(KOFERTA_SOURCES
    Database.cpp 
    DatabaseHelpers.cpp
    LoadDialog.cpp 
    MainWindow.cpp 
    User.cpp 
    Merchandise.cpp 
    MerchandiseSearch.cpp 
    MerchandiseSearchModel.cpp 
    MerchandiseSelection.cpp 
    MerchandiseSelectionModel.cpp 
    MerchendiseSelectionDelegate.cpp 
    MerchandiseListDelegate.cpp 
    MerchandiseListModel.cpp 
    CustomerNew.cpp 
    CustomerEdit.cpp 
    MerchandiseNew.cpp 
    CustomerSearch.cpp 
    OfferSearch.cpp 
    CustomerSelection.cpp 
    LoginDialog.cpp 
    AddConditionDialog.cpp 
    TermsChooserDialog.cpp 
    TermItem.cpp 
    TermModel.cpp 
    MerchandiseListView.cpp 
    LoadDialogMerchandiseListModel.cpp 
    Offer.cpp
    Customer.cpp
)
set(UI_FILES
    LoadDialog.ui
    MainWindow.ui
    MerchandiseSearch.ui
    CustomerNew.ui
    CustomerEdit.ui
    MerchandiseNew.ui
    OfferSearch.ui
    CustomerSelection.ui
    LoginDialog.ui
    AddConditionDialog.ui
    TermsChooserDialog.ui
)
set(TS_FILES
    translations/kOferta_pl.ts
    translations/kOferta_en.ts
    translations/kOferta_de.ts
)
qt5_add_translation(qm_files ${TS_FILES})

add_definitions(-DVERSION="${KOFERTA_VERSION}")
add_definitions(-DEXPORT_MAINWINDOW)
add_definitions(-DQT_MESSAGELOGCONTEXT)
add_definitions(-DSQL_USER="${KOFERTA_SQL_USER}")
add_definitions(-DSQL_PWD="${KOFERTA_SQL_PWD}")

IF(WIN32)
    add_definitions(-DWIN32)
    add_definitions(-DRELEASE)
ENDIF(WIN32)

add_library(mainwindow SHARED DatabaseHelpers.hpp ${KOFERTA_SOURCES} ${UI_FILES} res/zasoby.qrc ${qm_files})
set_property(TARGET  mainwindow PROPERTY CXX_STANDARD 14)
target_link_libraries(mainwindow ${QT_LIBRARIES})
