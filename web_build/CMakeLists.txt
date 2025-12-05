cmake_minimum_required(VERSION 3.13)
project(HelbreathRaylibWeb C CXX)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect all C/C++ source files in Sources/
file(GLOB_RECURSE SRC "${CMAKE_SOURCE_DIR}/../Sources/HelbreathRaylib/*.cpp" "${CMAKE_SOURCE_DIR}/../Sources/HelbreathRaylib/*.c")

add_executable(helbreath_web ${SRC})

# Your include dirs
target_include_directories(helbreath_web PRIVATE
    ${CMAKE_SOURCE_DIR}/../Sources/HelbreathRaylib
    ${CMAKE_SOURCE_DIR}/../Sources/Dependencies/Includes
    ${CMAKE_SOURCE_DIR}/../Sources/Dependencies/Shared
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
        "-fexceptions"
        "$<$<CONFIG:Release>:-O3>"
        "$<$<CONFIG:Debug>:-O0;-g>"
    )

    # Define platform and graphics API for raylib
    target_compile_definitions(helbreath_web PRIVATE
        PLATFORM_WEB
        GRAPHICS_API_OPENGL_ES2
    )

    # Add Emscripten linker flags
    target_link_options(helbreath_web PRIVATE
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
        "-sINITIAL_MEMORY=512MB"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFULL_ES2=1"
        "-sGL_ENABLE_GET_PROC_ADDRESS"
        "-sDISABLE_EXCEPTION_CATCHING=0"
        "-fexceptions"
        "-sSTACK_SIZE=5MB"
        "-sFORCE_FILESYSTEM=1"
        "--preload-file" "${CMAKE_SOURCE_DIR}/../Binaries/Game@/"
        "--shell-file" "${CMAKE_SOURCE_DIR}/shell.html"
        "-sLZ4=1"
        "-sEMULATE_FUNCTION_POINTER_CASTS=1"
        "-sDEMANGLE_SUPPORT=1"
        "$<$<CONFIG:Release>:-O3;-sASSERTIONS=1>"
        "$<$<CONFIG:Debug>:-O0;-g;-sASSERTIONS=2;-sSAFE_HEAP=1>"
    )

    # Output must be .html for Emscripten
    set_target_properties(helbreath_web PROPERTIES SUFFIX ".html")
else()
    # Path to your raylib static library for native builds
    set(RAYLIB_LIB "C:/Users/ShadowEvil/source/repos2/HelbreathRaylib/Sources/Dependencies/Libs/libraylib.a")
    target_link_libraries(helbreath_web PRIVATE ${RAYLIB_LIB})
endif()