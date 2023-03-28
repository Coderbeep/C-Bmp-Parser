cmake_minimum_required(VERSION 3.0.0)
project(bmp-parser VERSION 0.1.0)

include(CTest)
enable_testing()

file (GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "src/*.c")
file (GLOB_RECURSE MY_HEADERS CONFIGURE_DEPENDS "headers/*.h")

set (MY_INCLUDE_DIRS "")
foreach (_headerFile ${MY_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND MY_INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES MY_INCLUDE_DIRS)


add_executable(bmp-parser "src/main.c")
target_include_directories(bmp-parser PRIVATE ${MY_INCLUDE_DIRS})
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
