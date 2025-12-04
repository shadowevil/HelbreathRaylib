cmake_minimum_required(VERSION 3.13)
project(HelbreathRaylibWeb C CXX)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect all C/C++ source files in Sources/
file(GLOB_RECURSE SRC "Sources/HelbreathRaylib/*.cpp" "Sources/HelbreathRaylib/*.c")

add_executable(helbreath_web ${SRC})

# Your include dirs
target_include_directories(helbreath_web PRIVATE
    Sources/HelbreathRaylib
    Sources/Dependencies/Includes
    Sources/Dependencies/Shared
)

# Emscripten-specific settings
if(EMSCRIPTEN)
    # Link the Emscripten-built raylib library
    if(DEFINED RAYLIB_PATH)
        set(RAYLIB_LIB "${RAYLIB_PATH}/libraylib.a")
    else()
        set(RAYLIB_LIB "C:/Users/ShadowEvil/source/repos/emsdk/raylib/build_web/raylib/libraylib.a")
    endif()
    target_link_libraries(helbreath_web PRIVATE ${RAYLIB_LIB})

    # Add Emscripten compiler flags
    target_compile_options(helbreath_web PRIVATE
        "-O3"
        "-fexceptions"
    )

    # Define platform and graphics API for raylib
    target_compile_definitions(helbreath_web PRIVATE
        PLATFORM_WEB
        GRAPHICS_API_OPENGL_ES2
    )

    # Add Emscripten linker flags
    target_link_options(helbreath_web PRIVATE
        "-O3"
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
        "-sINITIAL_MEMORY=512MB"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFULL_ES2=1"
        "-sGL_ENABLE_GET_PROC_ADDRESS"
        "-sDISABLE_EXCEPTION_CATCHING=0"
        "-fexceptions"
        "-sSTACK_SIZE=5MB"
        "-sASSERTIONS=1"
        "-sFORCE_FILESYSTEM=1"
        "--preload-file" "${CMAKE_SOURCE_DIR}/Binaries/Game@/"
        "--shell-file" "${CMAKE_SOURCE_DIR}/shell.html"
    )

    # Output must be .html for Emscripten
    set_target_properties(helbreath_web PROPERTIES SUFFIX ".html")
else()
    # Path to your raylib static library for native builds
    set(RAYLIB_LIB "C:/Users/ShadowEvil/source/repos2/HelbreathRaylib/Sources/Dependencies/Libs/libraylib.a")
    target_link_libraries(helbreath_web PRIVATE ${RAYLIB_LIB})
endif()