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
        "external/vma/include"
        "/Users/samuliak/Documents/lava_core/include/vulkan"
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
        "external/metal-cpp/include"
        "/Users/samuliak/Documents/lava_core/include/metal"
    )
endif ()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${BACKEND})

file(GLOB src
    "src/camera/*.cpp"
    "src/entity/*.cpp"
    "src/skylight/*.cpp"
    "src/scripting/*.cpp"
    "src/physics/*.cpp"
)

add_library(${PROJECT_NAME} SHARED
    ${src}
)

#set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

find_package(Bullet REQUIRED)
#message("Bullet include: '${BULLET_INCLUDE_DIRS}'")
#message("Bullet lib: '${BULLET_LIBRARIES}'")

find_package(Assimp REQUIRED)
#message("Assimp include: '${ASSIMP_INCLUDE_DIRS}'")
#message("Assimp lib: '${ASSIMP_LIBRARIES}'")

include_directories(
    "/Users/samuliak/Documents/lvnd/include"
    "/usr/local/Cellar/glm/0.9.9.8/include"
    "external/stb/include"
    "external/entt/include"
    "include"
    ${ASSIMP_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
)

find_library(
    LVND_LIB
    NAMES lvnd liblvnd
    HINTS "/Users/samuliak/Documents/lvnd/lib/${BACKEND}"
    NO_DEFAULT_PATH
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${ASSIMP_LIBRARIES}
    ${LVND_LIB}
    ${BULLET_LIBRARIES}
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
        HINTS "/Users/samuliak/Documents/lava_core/lib/vulkan"
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
        HINTS "/Users/samuliak/Documents/lava_core/lib/metal"
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
