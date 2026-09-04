include(GNUInstallDirs)

set(LIB_BPLUSTREE_SRC bplustree.c)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_definitions(-D_BPLUS_TREE_DEBUG)

# ---- shared library ----
add_library(${LIB_BPLUSTREE_NAME} SHARED ${LIB_BPLUSTREE_SRC})
set_target_properties(${LIB_BPLUSTREE_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${LIB_BPLUSTREE_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)
set_target_properties(${LIB_BPLUSTREE_NAME} PROPERTIES PUBLIC_HEADER bplustree.h)

# ---- static library ----
add_library(${LIB_BPLUSTREE_NAME}_static STATIC ${LIB_BPLUSTREE_SRC})
set_target_properties(${LIB_BPLUSTREE_NAME}_static PROPERTIES OUTPUT_NAME "${LIB_BPLUSTREE_NAME}")
set_target_properties(${LIB_BPLUSTREE_NAME}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${LIB_BPLUSTREE_NAME}_static PROPERTIES PUBLIC_HEADER bplustree.h)

# Header search path: point at the source tree while building, and at the
# installed include/ dir after install. This makes consumers of the exported
# targets (find_package(bplustree)) find "bplustree.h" automatically.
foreach(t ${LIB_BPLUSTREE_NAME} ${LIB_BPLUSTREE_NAME}_static)
        target_include_directories(${t} PUBLIC
                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
endforeach()

# ---- install rules ----
set(BPLUSTREE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/bplustree)

install(TARGETS ${LIB_BPLUSTREE_NAME} ${LIB_BPLUSTREE_NAME}_static
        EXPORT bplustreeTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# ---- find_package(bplustree) support ----
include(CMakePackageConfigHelpers)
configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/bplustreeConfig.cmake.in
        ${PROJECT_BINARY_DIR}/bplustreeConfig.cmake
        INSTALL_DESTINATION ${BPLUSTREE_INSTALL_CMAKEDIR})
write_basic_package_version_file(
        ${PROJECT_BINARY_DIR}/bplustreeConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)
install(EXPORT bplustreeTargets
        FILE bplustreeTargets.cmake
        NAMESPACE bplustree::
        DESTINATION ${BPLUSTREE_INSTALL_CMAKEDIR})
install(FILES
        ${PROJECT_BINARY_DIR}/bplustreeConfig.cmake
        ${PROJECT_BINARY_DIR}/bplustreeConfigVersion.cmake
        DESTINATION ${BPLUSTREE_INSTALL_CMAKEDIR})
