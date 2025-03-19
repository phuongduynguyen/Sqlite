cmake_minimum_required(VERSION 3.5)
project(DatabaseRun LANGUAGES CXX)
set(CMAKE_PREFIX_PATH "/home/huy/Qt/6.5.3/gcc_64")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_AUTOMOC ON)
add_subdirectory(lib)
set(LIB_NAME DataBase)
find_package(Qt6 COMPONENTS Core Gui Quick REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

set(SOURCES
main.cpp
)

set(HEADERS
lib/include/DataEngine.h  
)

add_executable(app ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCES} ${HEADERS})
set_target_properties(app PROPERTIES AUTOMOC ON)
set_property(TARGET app PROPERTY CXX_STANDARD 17)

target_link_libraries(app ${LIB_NAME} Qt6::Core Qt6::Gui Qt6::Quick)

qt_add_resources(app "resources"
    PREFIX "/"
    FILES main.qml
)

include(GNUInstallDirs)

install(TARGETS DataBase app
LIBRARY 
DESTINATION ${CMAKE_INSTALL_LIBDIR}
RUNTIME 
DESTINATION ${CMAKE_INSTALL_BINDIR})

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/deploy)
set_target_properties(app PROPERTIES LINK_FLAGS "-rdynamic")