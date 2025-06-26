cmake_minimum_required(VERSION 3.10)

project(DbSqlExport)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui AxContainer Sql Network WebSockets)


file(GLOB_RECURSE SOURCES 
"${CMAKE_SOURCE_DIR}/source/*.cpp"
"${CMAKE_SOURCE_DIR}/source/*.h"
) # Добавьте сюда ваши файлы


qt6_wrap_ui(GLOB_RECURSE UI_HEADERS 
"${CMAKE_SOURCE_DIR}/source/*.ui"
)

qt6_add_resources(RESOURCES "${CMAKE_SOURCE_DIR}/source/DbSqlExport.qrc")

qt6_wrap_cpp(MOC_SOURCES ${SOURCES})

add_executable(DbSqlExport_APP ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${RESOURCES})

target_link_libraries(DbSqlExport_APP PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui Qt6::AxContainer Qt6::Sql Qt6::Network Qt6::WebSockets)