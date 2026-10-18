# SPDX-License-Identifier: GPL-3.0-or-later
# This file is a subdirectory of the original PC port's CMake project.
# Append to pc_port/CMakeLists.txt after add_executable(SilentHillPC):
# add_subdirectory("<ABSOLUTE_PATH_TO_THIS_FOLDER>" "${CMAKE_BINARY_DIR}/quick_weapons_mod")
# Use the SAME engine revision as the tested build.

if(NOT TARGET SilentHillPC)
    message(FATAL_ERROR "Quick Weapons must be added inside the PC port CMake build")
endif()

add_library(quick_weapons SHARED src/quick_weapons.c)
target_compile_definitions(quick_weapons PRIVATE
    SH_PC_PORT VER_${SH_VERSION} SKIP_ASM static_assert=_Static_assert)
target_include_directories(quick_weapons PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/psyq_compat"
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/../include"
    "${CMAKE_SOURCE_DIR}/../include/decomp"
    "${PSYCROSS_DIR}/include/psx"
    "${PSYCROSS_DIR}/include")

# SilentHillPC is built with ENABLE_EXPORTS; on Windows this links against its
# generated import library, providing references to engine globals/functions.
target_link_libraries(quick_weapons PRIVATE SilentHillPC)
set_target_properties(quick_weapons PROPERTIES
    PREFIX "" OUTPUT_NAME "quick_weapons"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/quick_weapons_release/plugins"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/quick_weapons_release/plugins")
