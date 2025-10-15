cmake_minimum_required(VERSION 3.10)
project(MOBA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prefer GLVND over legacy OpenGL
cmake_policy(SET CMP0072 NEW)

# --- Find packages ---
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(GLEW REQUIRED)

# Box2D via pkg-config
pkg_check_modules(BOX2D REQUIRED box2d)

# --- Collect include directories ---
set(PROJECT_INCLUDES
    /usr/include                 # ENet system include
    ${OPENGL_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${BOX2D_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
)

# --- Collect all source and header files ---
file(GLOB_RECURSE ALL_CPP_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE ALL_HPP_FILES "${PROJECT_SOURCE_DIR}/include/*.hpp")

# --- Create reusable Server library ---
file(GLOB_RECURSE SERVERLIB_SOURCES "${PROJECT_SOURCE_DIR}/src/Networking/Server/*.cpp")

add_library(ServerLib STATIC ${SERVERLIB_SOURCES})

# ✅ Ensure it has *all* include directories, just like executables
target_include_directories(ServerLib PUBLIC ${PROJECT_INCLUDES})

# --- Remove server sources from global list to avoid double compilation ---
list(REMOVE_ITEM ALL_CPP_FILES ${SERVERLIB_SOURCES})

# --- Separate out main.cpp files ---
file(GLOB_RECURSE MAIN_FILES
    "${PROJECT_SOURCE_DIR}/src/*/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

# --- Shared (non-main) sources ---
set(SHARED_SOURCES ${ALL_CPP_FILES})
list(REMOVE_ITEM SHARED_SOURCES ${MAIN_FILES})

# --- Automatically create one executable per main.cpp ---
foreach(main_file ${MAIN_FILES})
    # Get parent directory name (used as executable name)
    get_filename_component(dir ${main_file} DIRECTORY)
    get_filename_component(name ${dir} NAME)

    # Fallback to project name if it's the root src/main.cpp
    if("${name}" STREQUAL "src")
        set(name "MOBA")
    endif()

    message(STATUS "Creating executable: ${name}")

    add_executable(${name} ${main_file} ${SHARED_SOURCES} ${ALL_HPP_FILES})

    # ✅ Apply include directories to executables too
    target_include_directories(${name} PRIVATE ${PROJECT_INCLUDES})

    target_link_libraries(${name}
        ServerLib
        ${OPENGL_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${BOX2D_LIBRARIES}
        enet
        ${GLEW_LIBRARIES}
    )
endforeach()
