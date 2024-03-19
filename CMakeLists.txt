cmake_minimum_required(VERSION 3.17)

project(QtRemoteObjectsIPCDemo VERSION 0.0.1.0)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Concurrent RemoteObjects REQUIRED)

add_subdirectory(child)

add_subdirectory(parent)