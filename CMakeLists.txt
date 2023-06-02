cmake_minimum_required(VERSION 3.25)
project(Nasha
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(Nasha
        Sandbox/src/main.cpp
        Nasha/src/Nasha/Application.cpp
        Nasha/src/Nasha/Application.h
        Nasha/src/Nasha/Core.h
        Nasha/src/Nasha/Window.h
        Nasha/src/Nasha/Window.cpp
        Nasha/src/Nasha/EntryPoint.h
        Nasha/src/Nasha.h
        Nasha/src/Nasha/Device.cpp
        Nasha/src/Nasha/Device.h)

# Setting up Vulkan
find_package(Vulkan REQUIRED)
set(VULKAN_SDK_PATH /Users/nandysharma/CLionProjects/Nasha/Nasha/vendors/Vulkan)
target_include_directories(${PROJECT_NAME} PUBLIC ${VULKAN_SDK_PATH}/include)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

# Setting up GLFW
set(GLFW_PATH /Users/nandysharma/CLionProjects/Nasha/Nasha/vendors/GLFW)
if (DEFINED GLFW_PATH)
    set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
    set(GLFW_LIB "${GLFW_PATH}/lib")
else()
    find_package(glfw)
    set(GLFW_LIB glfw)
    message(STATUS "Found GLFW")
endif()
target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GLFW_LIB}/libglfw.3.3.dylib "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")


#setting up sdplog
set(SPDLOG_PATH ${PROJECT_SOURCE_DIR}/Nasha/vendors/spdlog)
set(SPDLOG_INCLUDE_DIRS "${SPDLOG_PATH}/include")
target_include_directories(${PROJECT_NAME} PUBLIC ${SPDLOG_INCLUDE_DIRS})