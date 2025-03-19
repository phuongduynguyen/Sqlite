# Build with QT: cmake -B build . -DBUILD_QT=ON
# Build without QT: cmake -B build . -DBUILD_QT=OFF


cmake_minimum_required(VERSION 3.5)
project(DatabaseRun LANGUAGES CXX)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_subdirectory(lib)
set(LIB_NAME DataBase)


option(BUILD_QT "Build with QT" OFF)

message(STATUS "BUILD_QT: ${BUILD_QT}")

if (BUILD_QT)
    add_definitions(-DBUILD_QT)
    message(STATUS "Building for QT")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Quick)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/include)


set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

if(BUILD_QT)
    list(APPEND SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
endif()

add_executable(run ${SOURCE_FILES})

set_target_properties(run PROPERTIES AUTOMOC ON)
set_property(TARGET run PROPERTY CXX_STANDARD 17)

target_link_libraries(run ${LIB_NAME})

if (BUILD_QT)
    target_link_libraries(run Qt6::Core Qt6::Gui Qt6::Quick)
    qt_add_resources(run "resources"
        PREFIX "/"
        FILES main.qml
    )
endif()

include(GNUInstallDirs)

install(TARGETS DataBase run
LIBRARY 
DESTINATION ${CMAKE_INSTALL_LIBDIR}
RUNTIME 
DESTINATION ${CMAKE_INSTALL_BINDIR})

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/deploy)
set_target_properties(run PROPERTIES LINK_FLAGS "-rdynamic")