# Build with QT: cmake -B build . -DBUILD_QT=ON
# Build without QT: cmake -B build . -DBUILD_QT=OFF

cmake_minimum_required(VERSION 3.5)
project(DatabaseApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_PREFIX_PATH "~/Qt/6.5.3/gcc_64/lib/cmake")

add_subdirectory(lib)
set(LIB_NAME DataBase)
set(Execute app)

option(BUILD_QT "Build with QT" OFF)
message(STATUS "BUILD_QT: ${BUILD_QT}")

set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

if (BUILD_QT)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Quick)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)    

    add_subdirectory(model)
    add_definitions(-DBUILD_QT)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

if (BUILD_QT)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/model/include)
endif()

add_executable(${Execute} ${SOURCE_FILES})

set_property(TARGET ${Execute} PROPERTY CXX_STANDARD 17)
target_link_libraries(${Execute} PRIVATE ${LIB_NAME})

if (BUILD_QT)
    target_link_libraries(${Execute} PRIVATE ${MODEL_LIB_NAME} Qt6::Core Qt6::Gui Qt6::Quick)
    qt_add_resources(${Execute} "resources"
        PREFIX "/"
        FILES main.qml
    )
endif()

include(GNUInstallDirs)

install(TARGETS ${Execute}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_QT)
    install(TARGETS ${MODEL_LIB_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/deploy)
set_target_properties(${Execute} PROPERTIES LINK_FLAGS "-rdynamic")
