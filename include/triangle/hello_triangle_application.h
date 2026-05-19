#pragma once
#include<vulkan/vulkan_raii.hpp>

struct GLFWwindow;
constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;


#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif // NDEBUG


class HelloTriangleApplication {
public:
	const std::vector<char const*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};
	std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };
	GLFWwindow* window;
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	HelloTriangleApplication();
	~HelloTriangleApplication();
	void run();
private:
	void initWindow();
	void initVulkan();
	void createInstance();
	void setupDebugMessenger();
	void pickPhysicalDevice();
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
	void createLogicalDevice();
	void createSurface();
	void mainLoop();
	void cleanup();

};