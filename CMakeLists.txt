cmake_minimum_required(VERSION 2.8)
project(qtable)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -g")

# Boost.1.58 using / declaring auto_ptr
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

#set(CMAKE_CXX_COMPILER "clang++")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -lc++ -stdlib=libc++")

find_package(Boost REQUIRED COMPONENTS regex system serialization iostreams)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt5Widgets REQUIRED)
qt5_add_resources(UI_RESOURCES resources.qrc)
add_executable(qtable main.cc mainwindow.cc model.cc model.h html_table.cc html_table.h interfaces.h filter.cc instrument.cc udp_server.h ${UI_RESOURCES})
target_link_libraries(qtable ${Boost_LIBRARIES})
target_link_libraries(qtable pthread) #Boost_USE_MULTITHREADED does not work on all linux distributions

#qt5_use_modules(qtable Widgets WebKitWidgets)
qt5_use_modules(qtable Widgets WebEngineWidgets WebChannel)
# specific kit: -DCMAKE_PREFIX_PATH=~/Qt5.6.2/5.6/gcc_64/lib/cmake/

add_executable(instrument_feeder instrument_feeder.cc udp_client.h proto.h types.h)
target_link_libraries(instrument_feeder ${Boost_LIBRARIES} pthread)

add_executable(price_feeder price_feeder.cc udp_client.h proto.h types.h)
target_link_libraries(price_feeder ${Boost_LIBRARIES} pthread)
