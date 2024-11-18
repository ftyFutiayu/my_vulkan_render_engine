cmake_minimum_required(VERSION 3.22)

add_library(adiosy_platform
        Private/AdLog.cpp
        Private/AdWindow.cpp
        Private/Window/AdGLFWwindow.cpp

        Private/AdGraphicContext.cpp
        Private/Graphic/AdVKGraphicContext.cpp
        Private/Graphic/AdVkDevice.cpp
        Private/Graphic/AdQueue.cpp
)

target_include_directories(adiosy_platform PUBLIC External)

# glfw
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(External/glfw)
target_link_libraries(adiosy_platform PRIVATE glfw)

# Vulkan
find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    message("----> Find vulkan success: ${Vulkan_INCLUDE_DIRS}")
endif ()
target_include_directories(adiosy_platform PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(adiosy_platform PRIVATE ${Vulkan_LIBRARY})