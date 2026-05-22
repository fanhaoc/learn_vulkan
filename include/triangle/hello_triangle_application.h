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
	uint32_t queueIndex = ~0;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::SwapchainKHR swapChain = nullptr;
	std::vector<vk::Image> swapChainImages;
	std::vector<vk::raii::ImageView> swapChainImageViews;
	vk::SurfaceFormatKHR swapChainSurfaceFormat;
	vk::Extent2D swapChainExtent;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::CommandPool commandPool = nullptr;
	vk::raii::CommandBuffer commandBuffer = nullptr;
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
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities);
	uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);
	void createSwapChain();
	void createImageViews();
	void createGraphicsPipeline();
	void createCommandPool();
	void createCommandBuffer();
	void recordCommandBuffer(uint32_t imageIndex);
	void transition_image_layout(
		uint32_t imageIndex,
		vk::ImageLayout old_layout,
		vk::ImageLayout new_layout,
		vk::AccessFlags2 src_access_mask,
		vk::AccessFlags2 dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask);
	void mainLoop();
	void cleanup();

};