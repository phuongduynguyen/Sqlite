set(MODEL_LIB_NAME ModelLib)
set(MODEL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/HmiInterface.cpp)
set(MODEL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/HmiInterface.h)
set(LIB_NAME DataBase)

add_library(${MODEL_LIB_NAME} STATIC ${MODEL_SOURCES} ${MODEL_HEADERS})

target_include_directories(${MODEL_LIB_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Quick)
target_link_libraries(${MODEL_LIB_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Quick)
set_target_properties(${MODEL_LIB_NAME} PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
)

target_link_libraries(${MODEL_LIB_NAME} PRIVATE ${LIB_NAME})

set(MODEL_LIB_NAME ${MODEL_LIB_NAME} PARENT_SCOPE)