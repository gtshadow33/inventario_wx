cmake_minimum_required(VERSION 3.16)
project(InventarioWx CXX)

set(CMAKE_CXX_STANDARD 17)

# ---- wxWidgets ----
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# ---- SQLite3 ----
find_package(SQLite3 REQUIRED)

# ---- Fuentes ----
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# ---- Includes ----
target_include_directories(${PROJECT_NAME} PRIVATE src/headers)

# ---- Links ----
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${wxWidgets_LIBRARIES}
    SQLite::SQLite3
)