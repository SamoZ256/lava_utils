cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME lavautils)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)

add_compile_options(
    -O3
    -Wno-nullability-completeness -Wno-deprecated-volatile
    -fPIC
)

option(BACKEND_VULKAN "Build using vulkan backend" OFF)
option(BACKEND_METAL "Build using metal backend" OFF)

set(BACKEND)

if (BACKEND_VULKAN)
    message("Building using vulkan backend")

    set(BACKEND vulkan)

    include_directories(
        "/Users/samuliak/Documents/VMA/include/VMA"
        "/Users/samuliak/Documents/lava_core/1.3/include/vulkan"
    )

    if (APPLE)
        include_directories(
            "/Users/samuliak/VulkanSDK/1.3.236.0/macOS/include"
        )
    elseif (WIN32)
        find_package(Vulkan REQUIRED)
    endif ()
elseif (BACKEND_METAL)
    message("Building using metal backend")

    set(BACKEND metal)

    include_directories(
        "/Users/samuliak/Documents/metal-cpp"
        "/Users/samuliak/Documents/lava_core/1.3/include/metal"
    )
endif ()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${BACKEND})

file(GLOB src
    "src/camera/*.cpp"
    "src/entity/*.cpp"
    "src/skylight/*.cpp"
    "src/scripting/*.cpp"
)

add_library(${PROJECT_NAME} SHARED
    ${src}
)

#set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

include_directories(
    "/Users/samuliak/Documents/lvnd/include"
    "/opt/homebrew/Cellar/glm/0.9.9.8/include"
    "/Users/samuliak/Documents/stb/include"
    "/opt/homebrew/Cellar/assimp/5.2.5/include"
    "/Users/samuliak/Documents/entt/include"
    "include"
)

find_library (
    ASSIMP_LIB
    NAMES assimp libassimp # what to look for
    HINTS "/opt/homebrew/Cellar/assimp/5.2.5/lib" # where to look
)

find_library(
    LVND_LIB
    NAMES lvnd liblvnd
    HINTS "/Users/samuliak/Documents/lvnd/lib/${BACKEND}"
    NO_DEFAULT_PATH
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${ASSIMP_LIB}
    ${LVND_LIB}
)

if (BACKEND_VULKAN)
    if (APPLE)
        find_library (
            Vulkan_LIBRARIES
            NAMES vulkan libvulkan # what to look for
            HINTS "/Users/samuliak/VulkanSDK/1.3.236.0/macOS/lib" # where to look
        )
    endif ()

    find_library (
        LAVA_CORE_LIB
        NAMES lavacore liblavacore
        HINTS "/Users/samuliak/Documents/lava_core/1.3/lib/vulkan"
        NO_DEFAULT_PATH
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
        ${Vulkan_LIBRARIES}
        ${LAVA_CORE_LIB}
    )
elseif (BACKEND_METAL)
    find_library(NS_FOUNDATION_LIB Foundation)
    find_library(QUARTZCORE_LIB QuartzCore)
    find_library(METAL_LIB Metal)

    find_library (
        LAVA_CORE_LIB
        NAMES lavacore liblavacore
        HINTS "/Users/samuliak/Documents/lava_core/1.3/lib/metal"
        NO_DEFAULT_PATH
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
        ${NS_FOUNDATION_LIB}
        ${QUARTZCORE_LIB}
        ${METAL_LIB}
        ${LAVA_CORE_LIB}
    )
endif ()

add_compile_options(
    -ldl -lpthread -lX11
)
