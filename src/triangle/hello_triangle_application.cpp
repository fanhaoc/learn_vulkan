#include "triangle/hello_triangle_application.h"
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include <iostream>


void HelloTriangleApplication::run() {
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void HelloTriangleApplication::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void HelloTriangleApplication::initVulkan() {
	createInstance();
}

void HelloTriangleApplication::createInstance() {
	vk::ApplicationInfo appInfo{
		.pApplicationName = "Hello Triangle",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = vk::ApiVersion14
	};

	// 验证是否支持所有需要的扩展
	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	// Check if the required GLFW extensions are supported by the Vulkan implementation.
	auto extensionProperties = context.enumerateInstanceExtensionProperties();
	auto unsupportPropertyIt = std::ranges::find_if(requiredExtensions, [&extensionProperties](auto const& requiredExtension) {
		return std::ranges::none_of(extensionProperties, [requiredExtension](auto const& extensionProperty) {
			return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
			});
		});
	if (unsupportPropertyIt != requiredExtensions.end()) {
		throw std::runtime_error("Required GLFW extension not supported: " + std::string(*unsupportPropertyIt));
	}

	// 验证是否支持所有需要的layer
	std::vector<char const*> requiredLayers;
	if (enabelValidationLayers) {
		requiredLayers.assign(validationLayers.begin(), validationLayers.end());
	}
	std::vector<vk::LayerProperties> layerProperties = context.enumerateInstanceLayerProperties();
	auto unsupportedLayerIt = std::ranges::find_if(requiredLayers, [&layerProperties](auto const& requiredLayer) {
		return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty) {
			return strcmp(layerProperty.layerName, requiredLayer) == 0;
			});
		});
	if (unsupportedLayerIt != requiredLayers.end()) {
		throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
	}

	vk::InstanceCreateInfo createInfo{
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data()
	};

	instance = vk::raii::Instance(context, createInfo);
}

void HelloTriangleApplication::mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

void HelloTriangleApplication::cleanup() {
	glfwDestroyWindow(window);

	glfwTerminate();
}