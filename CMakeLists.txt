cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(kOferta)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Network REQUIRED)
set(QT_LIBRARIES Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Network Qt5::PrintSupport)

include_directories(includes/)
set(KOFERTA_SOURCES
    src/Database.cpp 
    src/LoadDialog.cpp 
    src/Main.cpp 
    src/MainWindow.cpp 
    src/User.cpp 
    src/Logger.cpp 
    src/Merchandise.cpp 
    src/MerchandiseSearch.cpp 
    src/MerchandiseSearchModel.cpp 
    src/MerchandiseSelection.cpp 
    src/MerchandiseSelectionModel.cpp 
    src/MerchendiseSelectionDelegate.cpp 
    src/MerchandiseListDelegate.cpp 
    src/MerchandiseListModel.cpp 
    src/CustomerNew.cpp 
    src/CustomerEdit.cpp 
    src/MerchandiseNew.cpp 
    src/CustomerSearch.cpp 
    src/OfferSearch.cpp 
    src/CustomerSelection.cpp 
    src/SettingsDialog.cpp 
    src/LoginDialog.cpp 
    src/AddConditionDialog.cpp 
    src/TermsChooserDialog.cpp 
    src/TermItem.cpp 
    src/TermModel.cpp 
    src/MerchandiseListView.cpp 
    src/LoadDialogMerchandiseListModel.cpp 
    src/LoadDialogMerchandiseListModelMySQL.cpp
)
add_executable(koferta ${KOFERTA_SOURCES})

target_link_libraries(koferta ${QT_LIBRARIES})
