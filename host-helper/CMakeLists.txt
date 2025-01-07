cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(host-helper LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

qt_standard_project_setup()

qt_add_executable(elfloader3-helper src/main.cpp src/AppWindow.cpp src/KeyCodes.cpp)

target_link_libraries(elfloader3-helper PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)
target_include_directories(elfloader3-helper PUBLIC ../common)
