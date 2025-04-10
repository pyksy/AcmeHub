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
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql)

add_executable(AcmeHub
  main.cpp
  acmehubdatabase.h acmehubdatabase.cpp
)

string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
if (build_type STREQUAL release)
  target_compile_definitions(AcmeHub PUBLIC
    QT_NO_DEBUG_OUTPUT
  )
endif()

target_link_libraries(AcmeHub
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::HttpServer
  Qt${QT_VERSION_MAJOR}::Sql
)

include(GNUInstallDirs)
install(TARGETS AcmeHub
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
