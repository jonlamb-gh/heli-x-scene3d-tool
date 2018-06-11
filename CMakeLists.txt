cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(scene3d-tool VERSION 0.1 LANGUAGES C)

# src/lodepng.c is a symlink to vendor/lodepng/lodepng.cpp
add_executable(
    scene3d-tool
    src/lodepng.c
    src/view.c
    src/grid.c
    src/terrain.c
    src/actor.c
    src/gui.c
    src/main.c)

set_property(
    TARGET
    scene3d-tool
    PROPERTY
    C_STANDARD
    99)

target_compile_options(
    scene3d-tool
    PRIVATE
    -Wall
    -Wextra
    -Wshadow
    -Wno-unused-parameter
    -Wwrite-strings
    -Wstrict-prototypes
    -Wmissing-include-dirs
    -Wold-style-definition
    -Wnested-externs
    -Wmissing-include-dirs
    -Werror)

target_compile_definitions(
    scene3d-tool
    PRIVATE
    GL_GLEXT_PROTOTYPES
    GLX_GLXEXT_PROTOTYPES
    LODEPNG_NO_COMPILE_CPP)

target_include_directories(
    scene3d-tool
    PRIVATE
    include
    vendor/lodepng)

target_link_libraries(
    scene3d-tool
    -lglut
    -lGLU
    -lGL
    -lX11
    -lm
    -lpopt)

install(
    TARGETS
    scene3d-tool
    DESTINATION
    /usr/local/bin)

