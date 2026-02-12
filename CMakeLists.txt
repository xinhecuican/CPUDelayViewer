cmake_minimum_required(VERSION 3.16)

project(CPUDelayViewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Sql)

include(FetchContent)

set(ZSTD_BUILD_STATIC ON)
set(ZSTD_BUILD_SHARED OFF)
FetchContent_Declare(
    zstd
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    URL "https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz"
    SOURCE_SUBDIR build/cmake
)
FetchContent_MakeAvailable(zstd)

set(TS_FILES CPUDelayViewer_yue_CN.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        config.h
        InstViewer/instdefines.h
        DataLoader/dataloader.h
        DataLoader/dataloader.cpp
        DataLoader/cdsdataloader.h DataLoader/cdsdataloader.cpp
        InstViewer/instrow.h
        InstViewer/instrow.cpp
        InstViewer/instlabelitem.h
        InstViewer/instlabelitem.cpp
        InstViewer/instscene.h
        InstViewer/instscene.cpp
        InstViewer/instviewport.h
        InstViewer/instviewport.cpp
        InstViewer/instpanel.h
        InstViewer/instpanel.cpp
        InstViewer/insttoolbar.h
        InstViewer/insttoolbar.cpp
        InstViewer/instdialog.h
        InstViewer/instdialog.cpp
        InstViewer/instdelayheader.h
        InstViewer/instdelayheader.cpp
        Statistic/statrangedialog.h
        Statistic/statrangedialog.cpp
        Statistic/stattypewidget.h
        Statistic/stattypewidget.cpp
        Statistic/resulttypewidget.h
        Statistic/resulttypewidget.cpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CPUDelayViewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CPUDelayViewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(CPUDelayViewer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CPUDelayViewer
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(CPUDelayViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql libzstd_static)
target_include_directories(
    CPUDelayViewer
    PRIVATE
    ${zstd_SOURCE_DIR}/lib
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CPUDelayViewer)
endif()
set_target_properties(CPUDelayViewer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CPUDelayViewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CPUDelayViewer)
endif()
