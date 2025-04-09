cmake_minimum_required(VERSION 3.16)

project(AcmeHub LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS HttpServer)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS HttpServer)

add_executable(AcmeHub
  main.cpp
)
target_link_libraries(AcmeHub
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::HttpServer
)

include(GNUInstallDirs)
install(TARGETS AcmeHub
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
