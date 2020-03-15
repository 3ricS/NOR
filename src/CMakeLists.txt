# https://www.jetbrains.com/help/clion/qt-tutorial.html

cmake_minimum_required(VERSION 3.15)
project(NOR)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.14.1\\5.14.1\\mingw73_64")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Tell cmake to find the modules Qt5Core and Qt5widgets
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

include_directories(doc)
include_directories(src)
include_directories(img)

# set .h files
set(MOC_HEADERS
        defines.h
        model/component.h
        model/connection.h
        model/descriptionfield.h
        model/filemanager.h
        model/powersupply.h
        model/resistor.h
        view/editview.h
        view/mainwindow.h
        model/networkgraphics.h
        view/networkview.h
        view/powerview.h
        model/componentport.h
        model/node.h
        model/puzzlecalculator.h
        model/rowpiece.h
        model/commands.h
        )

# set .cpp files
set(SOURCES
        main.cpp
        model/component.cpp
        model/connection.cpp
        model/descriptionfield.cpp
        model/filemanager.cpp
        model/powersupply.cpp
        model/resistor.cpp
        view/editview.cpp
        view/mainwindow.cpp
        model/networkgraphics.cpp
        view/networkview.cpp
        view/powerview.cpp
        model/componentport.cpp
        model/puzzlecalculator.cpp
        model/rowpiece.cpp
        model/commands.cpp
        )

# set .ui files
set(UIS
        src/view/editview.ui
        src/view/mainwindow.ui
        src/view/powerview.ui
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
target_link_libraries(NOR Qt5::PrintSupport)