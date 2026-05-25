# Vulkan

### 一、 Vulkan基础流程

#### 1. 创建instance

通过`vk::raii::Instance()`构造函数初始化`instance`，构造函数接收两个参数:`vk::raii::Context`和`vk::InstanceCreateInfo`。

`context`执行默认初始化，`instanceCreateInfo`指定初始化需要五个成员`pApplicationInfo`,`enableExtensionCount`,`ppEnableExtensionNames`，`enabledLayerCount`, `ppEnabledLayerNames`。

- `pApplicationInfo`类型为`vk::ApplicationInfo`，作用是让驱动程序能清晰地识别出当前运行的应用程序和引擎，以便进行针对性的优化，或通过扩展获取高级功能。结构如下
```c++
strcut ApplicationInfo {
    VkStructureType    sType;                 // 必须是 VK_STRUCTURE_TYPE_APPLICATION_INFO
    const void*        pNext;                 // 用于扩展，通常为 nullptr
    const char*        pApplicationName;     // 应用程序名称
    uint32_t           applicationVersion;   // 应用程序版本
    const char*        pEngineName;          // 引擎名称 (可为 nullptr)
    uint32_t           engineVersion;        // 引擎版本 (若未使用引擎则为0)
    uint32_t           apiVersion;           // 应用程序目标 Vulkan API 版本
}
```
- `enableExtensionCount`和`ppEnableExtensionNames`用以指示需要使用的扩展，类型分别是`uint32_t`和`char const * const *`(字符串数组)
- `enabledLayerCount`,和`ppEnabledLayerNames`用以指示需要使用的层，类型分别是`uint32_t`和`char const * const *`。
常用的一类层就是验证层（Validation Layers），它可以在不修改程序的情况下，捕获到一些错误：API 使用错误、内存泄漏、同步问题和着色器问题等

```cpp
vk::raii::Context context;
vk::raii::Instance instance = nullptr;

constexpr vk::Application appInfo{
    .pApplicationName   = "Vulkan App",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName        = "No Engine",
    .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion         = vk::ApiVersion14
}

// 设置需要的layer
auto layerProperties = conetxt.enumerateInstanceLayerProperties(); // 获取vulkan所有支持的层
const std::vector<char const*> requiredLayers = { "VK_LAYER_KHRONOS_validation" }; // 设置需要的验证层
auto unsupportLayer = std::ranges::find_if(
    requiredLayers, [&layerProperties](auto const& requiredLayer){
        return std::ranges::none_of(
            layerProperties, [requiredLayer](auto const& layerProperty){
                return strcmp(layerProperty.layerName, requiredLayer) == 0;
            });
    }); 
if (unsupportedLayer != requiredLayers.end()) {
	throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
} // 查询是否支持所有需要的层

// 设置需要的扩展
uint32_t glfwExtensionCount = 0;
auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); 
std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount); // 获取gltf需要的扩展
requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName); // 验证层也需要这个扩展
auto extensionProperties = context.enumerateInstanceExtensionProperties();
	auto unsupportPropertyIt = std::ranges::find_if(requiredExtensions, [&extensionProperties](auto const& requiredExtension) {
		return std::ranges::none_of(extensionProperties, [requiredExtension](auto const& extensionProperty) {
			return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
			});
	});
if (unsupportPropertyIt != requiredExtensions.end()) {
	throw std::runtime_error("Required GLFW extension not supported: " + std::string(*unsupportPropertyIt));
} // 查询是否支持所有需要的扩展

// 创建createInfo
vk::InstanceCreateInfo createInfo{
	.pApplicationInfo = &appInfo,
	.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
	.ppEnabledLayerNames = requiredLayers.data(),
	.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
	.ppEnabledExtensionNames = requiredExtensions.data()
};

// 创建instance
instance = vk::raii::Instance(context, createInfo);

```
`context`是Vulkan-Hpp为了处理函数指针加载和简化初始化引入的对 Vulkan 全局级别函数的封装和管理器。

