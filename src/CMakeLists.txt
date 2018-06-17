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
    OfferSearchModel.cpp
)

#if(CMAKE_EXTRA_GENERATOR OR MSVC_IDE)
    message(STATUS "Adding header files to project")
    file(GLOB_RECURSE KOFERTA_HEADERS "${KOFERTA_INCLUDE_DIR}/*.hpp")
    list(APPEND KOFERTA_SOURCES ${KOFERTA_HEADERS})
#endif()

add_definitions(-DVERSION="${KOFERTA_VERSION}")
add_definitions(-DEXPORT_MAINWINDOW)
add_definitions(-DQT_MESSAGELOGCONTEXT)
add_definitions(-DSQL_USER="${KOFERTA_SQL_USER}")
add_definitions(-DSQL_PWD="${KOFERTA_SQL_PWD}")

IF(WIN32)
    add_definitions(-DWIN32)
    add_definitions(-DRELEASE)
ENDIF(WIN32)

add_library(mainwindow SHARED ${KOFERTA_SOURCES})# ${UI_GENERATED_HEADERS})
#set_property(TARGET  mainwindow PROPERTY CXX_STANDARD 14)
target_link_libraries(mainwindow Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Network Qt5::PrintSupport)

IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF(WIN32)

add_executable(koferta ${GUI_TYPE} Main.cpp ${KOFERTA_RESOURCES}) 
target_link_libraries(koferta mainwindow logger)

foreach(TARGET
        mainwindow
        koferta
)
    target_include_directories(${TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    )
endforeach()
