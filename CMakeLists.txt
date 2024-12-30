cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tank)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
link_libraries(${SDL2_LIBRARIES} SDL2_image SDL2_gfx SDL2_ttf)

add_executable(${PROJECT_NAME} src/main.cpp 
src/utils.cpp)
