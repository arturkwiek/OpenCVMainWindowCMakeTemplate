cmake_minimum_required(VERSION 3.5)

project(OpenCVMainWindowCMakeTemplate VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Znajdź pakiet OpenCV
set(OpenCV_DIR "C:/opencv-4.9.0/build/")

find_package(OpenCV REQUIRED)

# Sprawdź, czy OpenCV zostało znalezione
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found. Please check your installation.")
else()
    message(STATUS "OpenCV found: ${OpenCV_INCLUDE_DIRS}")    
    message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
endif()

include_directories(${OpenCV_INCLUDE_DIRS})

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OpenCVMainWindowCMakeTemplate
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        videocapturethread.h
        videocapturethread.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET OpenCVMainWindowCMakeTemplate APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(OpenCVMainWindowCMakeTemplate SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(OpenCVMainWindowCMakeTemplate
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Link Qt and OpenCV libraries
target_link_libraries(OpenCVMainWindowCMakeTemplate PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${OpenCV_LIBS})


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.OpenCVMainWindowCMakeTemplate)
endif()
set_target_properties(OpenCVMainWindowCMakeTemplate PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS OpenCVMainWindowCMakeTemplate
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(OpenCVMainWindowCMakeTemplate)
endif()
