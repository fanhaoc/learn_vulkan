cmake_minimum_required(VERSION 3.10)
project(Learn_Vulkan)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")



find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan 1.4.335 REQUIRED)


add_executable(Learn_Vulkan "src/Learn_Vulkan.cpp" "include/triangle/hello_triangle_application.h" "src/triangle/hello_triangle_application.cpp")
target_link_libraries(Learn_Vulkan PRIVATE glfw Vulkan::Vulkan)
target_compile_definitions(Learn_Vulkan PRIVATE VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

target_include_directories(Learn_Vulkan PRIVATE src)
target_include_directories(Learn_Vulkan PUBLIC include)
