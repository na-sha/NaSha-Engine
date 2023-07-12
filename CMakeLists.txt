cmake_minimum_required(VERSION 3.25)
project(Nasha)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "using ${CMAKE_GENERATOR}")

if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles") # Only for windows
    if (NOT MINGW_PATH)
        message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()
    set(USE_MINGW "True")
    set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER  ${MINGW_PATH}/bin/g++.exe)
endif()

# Setting up Vulkan
set(VULKAN_SDK_PATH ${PROJECT_SOURCE_DIR}/Nasha/vendors/Vulkan)
if (DEFINED VULKAN_SDK_PATH)
    message(STATUS "Found Vulkan")
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include")
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib")
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

# Setting up GLFW
set(GLFW_PATH ${PROJECT_SOURCE_DIR}/Nasha/vendors/GLFW)
if (DEFINED GLFW_PATH)
    message(STATUS "GLFW_PATH found")
    set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
    set(GLFW_LIB "${GLFW_PATH}/lib")
else()
    find_package(glfw3 3.3 REQUIRED) # throws error if could not find GLFW
    set(GLFW_LIB glfw)
    message(STATUS "Found GLFW")
endif()
if (NOT GLFW_LIB)
    message(FATAL_ERROR "Could not find glfw library!")
else()
    message(STATUS "Using glfw lib at: ${GLFW_LIB}")
endif()

# Setting up tinyobjloader
set(TINY_OBJ_LOADER_PATH /Users/nandysharma/dev/NaSha-Engine/Nasha/vendors/tinyobjloader)

file(GLOB_RECURSE SOURCES
        ${PROJECT_SOURCE_DIR}/Nasha/src/*.cpp
        ${PROJECT_SOURCE_DIR}/Nasha/src/Nasha/*.cpp
        ${PROJECT_SOURCE_DIR}/Sandbox/src/Nasha/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_property(TARGET ${PROJECT_NAME} PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

if (WIN32)
    message(STATUS "CREATING BUILD FOR WINDOWS")

    if (USE_MINGW)
        target_include_directories(${PROJECT_NAME} PUBLIC
                ${MINGW_PATH}/include
                )
        target_link_directories(${PROJECT_NAME} PUBLIC
                ${MINGW_PATH}/lib
                )
    endif()

    target_include_directories(${PROJECT_NAME} PUBLIC
            ${PROJECT_SOURCE_DIR}/src
            ${VULKAN_INCLUDE_DIRS}
            ${TINYOBJ_PATH}
            ${GLFW_INCLUDE_DIRS}
            ${GLM_PATH}
            )

    target_link_directories(${PROJECT_NAME} PUBLIC
            ${VULKAN_LIBRARIES}
            ${GLFW_LIB}
            )

    target_link_libraries(${PROJECT_NAME} glfw3 vulkan-1)
elseif (UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")
    target_include_directories(${PROJECT_NAME} PUBLIC
            ${Vulkan_INCLUDE_DIRS}
            ${GLFW_INCLUDE_DIRS}
            ${TINY_OBJ_LOADER_PATH}
            )
    target_link_libraries(${PROJECT_NAME}
            ${Vulkan_LIBRARIES}/libvulkan.1.dylib
            ${Vulkan_LIBRARIES}/libvulkan.1.3.250.dylib
            ${GLFW_LIB}/libglfw.3.3.dylib
            "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
endif()
