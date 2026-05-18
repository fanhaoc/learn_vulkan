#pragma once
#include<vulkan/vulkan_raii.hpp>

struct GLFWwindow;
constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

const std::vector<char const*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enabelValidationLayers = true;
#endif // NDEBUG


class HelloTriangleApplication {
public:
	HelloTriangleApplication() {};
	~HelloTriangleApplication() {};
	void run();
private:
	GLFWwindow* window;
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	void initWindow();
	void initVulkan();
	void createInstance();
	void mainLoop();
	void cleanup();

};