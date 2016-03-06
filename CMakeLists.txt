cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(kOferta)

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

include_directories(include/)
set(KOFERTA_SOURCES
    Database.cpp 
    LoadDialog.cpp 
    Main.cpp 
    MainWindow.cpp 
    User.cpp 
    Logger.cpp 
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
    SettingsDialog.cpp 
    LoginDialog.cpp 
    AddConditionDialog.cpp 
    TermsChooserDialog.cpp 
    TermItem.cpp 
    TermModel.cpp 
    MerchandiseListView.cpp 
    LoadDialogMerchandiseListModel.cpp 
    LoadDialogMerchandiseListModelMySQL.cpp
)
#QT5_WRAP_UI(UI_HDRS
#    forms/LoadDialog.ui
#    forms/MainWindow.ui
#    forms/MerchandiseSearch.ui
#    forms/CustomerNew.ui
#    forms/CustomerEdit.ui
#    forms/MerchandiseNew.ui
#    forms/OfferSearch.ui
#    forms/CustomerSelection.ui
#    forms/SettingsDialog.ui
#    forms/LoginDialog.ui
#    forms/AddConditionDialog.ui
#    forms/TermsChooserDialog.ui
#)
set(TS_FILES
    translations/kOferta_pl.ts
    translations/kOferta_en.ts
    translations/kOferta_de.ts
)
qt5_add_translation(qm_files ${TS_FILES})

add_definitions(-DVER=2.61)
if(MSVC)
    add_definitions(-DWIN32)
    add_definitions(-DRELEASE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

add_executable(koferta ${KOFERTA_SOURCES} res/zasoby.qrc ${qm_files})
set_property(TARGET koferta PROPERTY CXX_STANDARD 11)
qt5_use_modules(koferta Widgets Sql Network PrintSupport)
target_link_libraries(koferta ${QT_LIBRARIES})
