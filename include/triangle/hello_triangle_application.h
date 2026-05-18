#pragma once
#include<vulkan/vulkan_raii.hpp>

struct GLFWwindow;
constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

const std::vector<char const*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif // NDEBUG


class HelloTriangleApplication {
public:
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
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
	void setupDebugMessenger();
	void pickPhysicalDevice();
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
	void mainLoop();
	void cleanup();

};