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

IF(WIN32)
    add_definitions(-DWIN32)
    add_definitions(-DRELEASE)
    SET(GUI_TYPE WIN32)
ENDIF()

add_library(mainwindow SHARED ${KOFERTA_SOURCES})# ${UI_GENERATED_HEADERS})
#set_property(TARGET  mainwindow PROPERTY CXX_STANDARD 14)
target_link_libraries(mainwindow Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Network Qt5::PrintSupport)
target_compile_definitions(mainwindow PRIVATE VERSION="${KOFERTA_VERSION}")
target_compile_definitions(mainwindow PRIVATE EXPORT_MAINWINDOW)
target_compile_definitions(mainwindow PRIVATE QT_MESSAGELOGCONTEXT)
target_compile_definitions(mainwindow PRIVATE SQL_USER="${KOFERTA_SQL_USER}")
target_compile_definitions(mainwindow PRIVATE SQL_PWD="${KOFERTA_SQL_PWD}")

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
