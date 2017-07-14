cmake_minimum_required(VERSION 2.8)
project(qtable)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Boost REQUIRED COMPONENTS regex system serialization iostreams)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt5Widgets REQUIRED)
qt5_add_resources(UI_RESOURCES resources.qrc)
add_executable(qtable main.cc mainwindow.cc ${UI_RESOURCES})
target_link_libraries(qtable ${Boost_LIBRARIES})
target_link_libraries(qtable pthread) #Boost_USE_MULTITHREADED does not work on all linux distributions

qt5_use_modules(qtable Widgets WebKitWidgets)
# >5.6 qt5_use_modules(qtable Widgets WebEngineWidgets)
# specific kit: -DCMAKE_PREFIX_PATH=~/Qt5.6.2/5.6/gcc_64/lib/cmake/
