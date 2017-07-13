cmake_minimum_required(VERSION 2.8)
project(qtable)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
qt5_add_resources(UI_RESOURCES resources.qrc)
add_executable(qtable main.cc mainwindow.cc ${UI_RESOURCES})

qt5_use_modules(qtable Widgets Network WebKitWidgets)
