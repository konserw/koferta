add_definitions(-DVERSION="${KOFERTA_VERSION}")
IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF()

set(MAINWINDOW_SOURCES
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
    OfferSearchModel.cpp
)
set(DATABASE_SOURCES Database.cpp DatabaseHelpers.cpp)
set(DATABASE_HEADERS "${KOFERTA_INCLUDE_DIR}/Database.hpp"
                     "${KOFERTA_INCLUDE_DIR}/DatabaseHelpers.hpp"
)
set(LOGGER_SOURCES Logger.cpp)
set(LOGGER_HEADERS "${KOFERTA_INCLUDE_DIR}/Logger.hpp")

message(STATUS "Adding header files to project")
file(GLOB_RECURSE MAINWINDOW_HEADERS "${KOFERTA_INCLUDE_DIR}/*.hpp")
list(REMOVE_ITEM MAINWINDOW_HEADERS ${LOGGER_HEADERS})
list(REMOVE_ITEM MAINWINDOW_HEADERS ${DATABASE_HEADERS})
list(APPEND MAINWINDOW_SOURCES ${MAINWINDOW_HEADERS})
list(APPEND DATABASE_SOURCES ${DATABASE_HEADERS})

add_library(logger STATIC Logger.cpp)
target_compile_definitions(logger PUBLIC QT_MESSAGELOGCONTEXT)
target_link_libraries(logger Qt5::Core)

add_library(database STATIC ${DATABASE_SOURCES})
target_link_libraries(database PUBLIC Qt5::Core Qt5::Sql Qt5::Network mainwindow)

add_library(mainwindow STATIC ${MAINWINDOW_SOURCES})
target_link_libraries(mainwindow Qt5::Sql Qt5::Core Qt5::Widgets Qt5::PrintSupport database)
target_compile_definitions(mainwindow PRIVATE EXPORT_MAINWINDOW)
target_compile_definitions(mainwindow PRIVATE SQL_USER="${KOFERTA_SQL_USER}")
target_compile_definitions(mainwindow PRIVATE SQL_PWD="${KOFERTA_SQL_PWD}")

add_executable(koferta ${GUI_TYPE} Main.cpp application.cpp application.h ${KOFERTA_RESOURCES})
target_link_libraries(koferta mainwindow logger)

