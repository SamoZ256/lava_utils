cmake_minimum_required(VERSION 3.10)

project(LavaEngineExamples)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(EXECUTABLE_OUTPUT_PATH "../")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#if(UNIX and not APPLE)
#	set(LINUX true)
#endif()

add_compile_options(
    -O2
)
#set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "main")

include_directories(
    "/usr/local/Cellar/Vulkan/macOS/include"
    "/usr/local/Cellar/glfw/3.3.7/include"
    "/usr/local/Cellar/glm/0.9.9.8/include"
    "/Users/samuliak/Documents/stb/include"
    "/Users/samuliak/Documents/VMA/include/VMA"
    "/usr/local/Cellar/assimp/5.2.4/include"
    "../include"
    "/Users/samuliak/Documents/PerlinNoise/include"
)

find_library(
    VULKAN_LIB
    NAMES vulkan libvulkan
    HINTS "/usr/local/Cellar/Vulkan/macOS/lib"
)

find_library(
    GLFW_LIB
    NAMES glfw libglfw
    HINTS "/usr/local/Cellar/glfw/3.3.7/lib"
)

find_library(
    LAVA_LIB
    NAMES LavaEngine libLavaEngine
    HINTS "../lib"
    NO_DEFAULT_PATH
)

message(STATUS "ENGINE_LIB: [${ENGINE_LIB}]")

file(GLOB EXAMPLE_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */*.cpp)

foreach(EXAMPLE_SOURCE_FILE ${EXAMPLE_SOURCE_FILES})
    string(REPLACE ".cpp" "" EXEC_NAME ${EXAMPLE_SOURCE_FILE})
    string(REPLACE "/" "" EXEC_NAME_FULL ${EXEC_NAME})
    #string(REPLACE "/" ";" EXEC_NAME_LIST ${EXEC_NAME})

    add_executable(${EXEC_NAME_FULL} ${EXAMPLE_SOURCE_FILE})

    target_link_libraries(${EXEC_NAME_FULL}
        ${VULKAN_LIB}
        ${GLFW_LIB}
        ${LAVA_LIB}
    )
endforeach(EXAMPLE_SOURCE_FILE)

if (UNIX)
    add_compile_options(
        -ldl -lpthread -lX11
    )
endif()
