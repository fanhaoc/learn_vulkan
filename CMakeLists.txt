cmake_minimum_required(VERSION 3.10)
project(Learn_Vulkan)
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan 1.4.335 REQUIRED)

add_executable(Learn_Vulkan "src/Learn_Vulkan.cpp" "include/triangle/hello_triangle_application.h" "src/triangle/hello_triangle_application.cpp" "include/data.cpp" "include/stb_image.h")
target_link_libraries(Learn_Vulkan PRIVATE glfw Vulkan::Vulkan)
target_compile_definitions(Learn_Vulkan PRIVATE VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

target_include_directories(Learn_Vulkan PRIVATE src)
target_include_directories(Learn_Vulkan PUBLIC include)
target_include_directories(Learn_Vulkan PUBLIC shaders)

# 将shaders复制到输出目录/cmake构建时
# file(GLOB SHADER_FILES shaders/spirv/*.spv)
# file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)

# 将shaders复制到输出目录/编译时
add_custom_command(TARGET Learn_Vulkan POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Learn_Vulkan>/shaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
	${CMAKE_SOURCE_DIR}/shaders/spirv 
	${CMAKE_CURRENT_BINARY_DIR}/shaders 
)

# 将资源复制到exe对应目录下
add_custom_command(TARGET Learn_Vulkan POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Learn_Vulkan>/public
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/public $<TARGET_FILE_DIR:Learn_Vulkan>/public
	COMMENT "custom: Copying public resources"
)
