cmake_minimum_required(VERSION 3.5)

project(DataBase LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)  # Match root project
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(LIB_NAME DataBase)
set(JPEG_LIBRARIES /lib/x86_64-linux-gnu/libjpeg.so)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

if (NOT SOURCES)
    message(FATAL_ERROR "No source files found in ${CMAKE_CURRENT_SOURCE_DIR}/src/")
endif()

add_compile_options(-fPIC)

add_library(${LIB_NAME} SHARED ${SOURCES})

target_include_directories(${LIB_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
find_package(Qt6 COMPONENTS Core Gui Quick REQUIRED)
target_link_libraries(${LIB_NAME} PUBLIC Qt6::Core Qt6::Gui Qt6::Quick)
find_package(SQLite3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(${LIB_NAME} PUBLIC 
Qt6::Core
    Qt6::Gui
    Qt6::Quick
    sqlite3
    OpenGL::GL
    GLEW::GLEW
    glfw
    ${JPEG_LIBRARIES}
    pthread
)
target_link_libraries(DataBase PUBLIC Qt6::Core Qt6::Gui Qt6::Quick)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")

install(TARGETS ${LIB_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)