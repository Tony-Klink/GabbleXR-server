cmake_minimum_required(VERSION 3.10)
project(gabble)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENXR REQUIRED openxr)
pkg_check_modules(LIBLO REQUIRED liblo)

add_executable(gabble main.cpp)
target_link_libraries(gabble PRIVATE ${OPENXR_LIBRARIES} ${LIBLO_LIBRARIES})
target_include_directories(gabble PRIVATE ${OPENXR_INCLUDE_DIRS} ${LIBLO_INCLUDE_DIRS})
