#pragma once
#define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS // 这个保证了当交换链过期时，vk::Result::eErrorOutOfDate会被当做成功处理，不然会直接抛出异常。
#include<vulkan/vulkan_raii.hpp>

struct GLFWwindow;
constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;
constexpr int MAX_FRAMES_IN_FLIGHT = 2; // 并行渲染数

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
	vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicPipeline = nullptr;
	vk::raii::CommandPool commandPool = nullptr;
	vk::raii::Image textureImage = nullptr;
	vk::raii::DeviceMemory textureImageMemory = nullptr;
	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory vertexBufferMemory = nullptr; // 这是为vertexbuffer申请到的内存
	vk::raii::Buffer indexBuffer = nullptr;
	vk::raii::DeviceMemory indexBufferMemory = nullptr; 
	std::vector<vk::raii::Buffer> uniformBuffers; // 需要为每一个commandbuffer都创建一个uniformbuffer
	std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped; // 这个是为了把uniformbuffer的内存映射到cpu上，方便更新数据
	vk::raii::DescriptorPool descriptorPool = nullptr;
	std::vector<vk::raii::DescriptorSet> descriptorSets;
	std::vector<vk::raii::CommandBuffer> commandBuffers;
	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> drawFences;
	uint32_t frameIndex = 0;
	bool frameBufferResized = false;
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
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createCommandPool();
	void createTextureImage();
	std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage(
		uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties
	);
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDecriptorSets();
	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
	void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);
	uint32_t findMemorytype(uint32_t typeFilter, vk::MemoryPropertyFlags properties); // typefilter表示允许使用哪些内存类型
	void createCommandBuffer();
	void createSyncObjects();
	void recordCommandBuffer(uint32_t imageIndex);
	vk::raii::CommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer);
	void transition_image_layout(
		uint32_t imageIndex,
		vk::ImageLayout old_layout,
		vk::ImageLayout new_layout,
		vk::AccessFlags2 src_access_mask,
		vk::AccessFlags2 dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask);
	void transitionImageLayout(
		vk::raii::CommandBuffer& commandBuffer,
		const vk::raii::Image& image,
		vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout
	);
	void copyBufferToImage(
		vk::raii::CommandBuffer& commandBuffer,
		const vk::raii::Buffer& buffer,
		vk::raii::Image& image,
		uint32_t width,
		uint32_t height
	);
	void mainLoop();
	void drawFrame();
	void updateUniformBuffer(uint32_t currentImage);
	void cleanup();
	void recreateSwapChain();
	void cleanupSwapChain();

	

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
		vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*
	);
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};