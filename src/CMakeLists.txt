# https://www.jetbrains.com/help/clion/qt-tutorial.html

cmake_minimum_required(VERSION 3.15)
project(NOR)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:\\Qt\\5.14.1\\mingw73_64")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Tell cmake to find the modules Qt5Core and Qt5widgets
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)

include_directories(doc)
include_directories(src)

# set .h files
set(MOC_HEADERS
        main.h
        model/component.h
        model/connection.h
        model/powersupply.h
        model/resistor.h
        model/networkgraphics.cpp

        view/mainwindow.h
        view/NetworkView.h
        )

# set .cpp files
set (SOURCES
        main.cpp
        model/component.cpp
        model/connection.cpp
        model/powersupply.cpp
        model/resistor.cpp
        model/networkgraphics.cpp

        view/mainwindow.cpp
        view/NetworkView.cpp)

# set .ui files
set(UIS
        src/mainwindow.ui
        )

# set ressources
set(RESOURCES
        rsrc/resources.qrc)


add_executable(NOR
        ${SOURCES}
        ${MOC_SRCS}
        ${RES_SOURCES}
        ${UI_HEADERS}
        )


target_link_libraries(NOR Qt5::Widgets)
target_link_libraries(NOR Qt5::Core)
target_link_libraries(NOR Qt5::Gui)