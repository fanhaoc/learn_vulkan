#pragma once
#include<vulkan/vulkan_raii.hpp>

struct GLFWwindow;
constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;
class HelloTriangleApplication {
public:
	HelloTriangleApplication() {};
	~HelloTriangleApplication() {};
	void run();
private:
	GLFWwindow* window;
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();

};