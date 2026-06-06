cmake_minimum_required(VERSION 3.10)
project(MOBA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define DEBUG in Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

# Prefer GLVND over legacy OpenGL
cmake_policy(SET CMP0072 NEW)

# ----------------------------
# Compiler flags
# ----------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Warnings (project-wide)
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
    )

    # Release-only "max" flags
    add_compile_options(
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-DNDEBUG>
        $<$<CONFIG:Release>:-march=native>
        $<$<CONFIG:Release>:-ffunction-sections>
        $<$<CONFIG:Release>:-fdata-sections>
    )

    add_link_options(
        $<$<CONFIG:Release>:-Wl,--gc-sections>
    )

    # Enable LTO/IPO in Release if supported
    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
    if(lto_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    else()
        message(STATUS "IPO/LTO not supported: ${lto_error}")
    endif()
endif()

# ----------------------------
# Find packages
# ----------------------------
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)

# ----------------------------
# Include directories
# ----------------------------
set(PROJECT_INCLUDES
    ${OPENGL_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external
)

# ----------------------------
# GLAD (build as its own library + suppress pedantic warnings there)
# ----------------------------
add_library(glad STATIC ${PROJECT_SOURCE_DIR}/src/glad.c)

# If your glad headers are in ${PROJECT_SOURCE_DIR}/include, this is correct.
# If they are elsewhere, add that path here too.
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/include)

# glad.c (generated) triggers -Wpedantic warnings; silence for this target only
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(glad PRIVATE -Wno-pedantic)
endif()

# ----------------------------
# Source collection
# ----------------------------
file(GLOB_RECURSE ALL_CPP_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Find all main.cpp files (one executable per main)
file(GLOB_RECURSE MAIN_FILES
    "${PROJECT_SOURCE_DIR}/src/*/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

# Shared sources = everything except mains
set(SHARED_SOURCES ${ALL_CPP_FILES})
list(REMOVE_ITEM SHARED_SOURCES ${MAIN_FILES})

# ----------------------------
# Build one executable per main.cpp
# ----------------------------
foreach(main_file ${MAIN_FILES})
    get_filename_component(dir ${main_file} DIRECTORY)
    get_filename_component(name ${dir} NAME)

    # If it's src/main.cpp at project root
    if("${name}" STREQUAL "src")
        set(name "MOBA")
    endif()

    message(STATUS "Creating executable: ${name}")

    add_executable(${name}
        ${main_file}
        ${SHARED_SOURCES}
    )

    target_include_directories(${name} PRIVATE ${PROJECT_INCLUDES})

    target_link_libraries(${name}
        glad
        ${OPENGL_LIBRARIES}
        ${SDL2_LIBRARIES}
        enet
        dl
    )
endforeach()