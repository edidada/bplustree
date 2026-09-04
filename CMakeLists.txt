cmake_minimum_required (VERSION 3.5)

project (bplustree VERSION 1.0 LANGUAGES C)

enable_testing()

include(GNUInstallDirs)

#set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(LIB_BPLUSTREE_NAME bplustree)

add_subdirectory(lib)
add_subdirectory(tests)
