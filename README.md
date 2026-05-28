# Vulkan

### 一、 Vulkan基础流程

#### 1. 创建instance

通过`vk::raii::Instance()`构造函数初始化`instance`，构造函数接收两个参数:`vk::raii::Context`和`vk::InstanceCreateInfo`。

`context`是Vulkan-Hpp为了处理函数指针加载和简化初始化引入的对 Vulkan 全局级别函数的封装和管理器，直接执行默认初始化，`instanceCreateInfo`指定初始化需要五个成员`pApplicationInfo`，`enableExtensionCount`，`ppEnableExtensionNames`，`enabledLayerCount`, `ppEnabledLayerNames`。

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

最终：

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
	throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayer));
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
#### 2. 选择物理设备
在通过`vk::raii::Instance`初始化Vulakn后，接下来需要查询电脑上所有的物理设备，并选择支持我们需要的特性的一个或多个物理设备。物理设备使用`vk::raii::PhysicalDevice`存放，
通过`instance.enumeratePhysicalDevices()`获取所有可用物理设备。
选择物理设备主要从这几个方面：
- 设备属性(properties)：设备的基本身份和各项资源的硬性限制，如身份标识、API/驱动版本、管线缓存标识、设备类型（独显、集显、CPU等）、硬性限制。其中硬性限制在实际开发中最常用，
比如`maxImageDimension2D`最大2D纹理尺寸、`maxUniformBufferRange`uniform buffer 最大字节数、`maxColorAttachments`最多同时渲染的颜色附件数量。**通过`vk::raii::PhysicalDevice::getProperties`
获取所有属性信息`PhysicalDeviceProperties`**。
```cpp
struct PhysicalDeviceProperties{
    uint32_t                                               apiVersion        = {};
    uint32_t                                               driverVersion     = {};
    uint32_t                                               vendorID          = {};
    uint32_t                                               deviceID          = {};
    PhysicalDeviceType                                     deviceType        = PhysicalDeviceType::eOther;
    ArrayWrapper1D<char, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE> deviceName        = {};
    ArrayWrapper1D<uint8_t, VK_UUID_SIZE>                  pipelineCacheUUID = {};
    PhysicalDeviceLimits                                   limits            = {};
    PhysicalDeviceSparseProperties                         sparseProperties  = {};
}
```
- 设备特性(features)：设备所支持的可选功能，**通过`vk::raii::PhysicalDevice::getFeatures`获取所有属性信息`PhysicalDeviceFeatures`**。这是一个装满布尔值(`VK_TRUE`和`VK_FALSE`)的结构体，
每个字段表示一种可选的高级图形/计算功能是否可用。这些功能默认是关闭的，必须在创建逻辑设备（VkDevice）时显式请求开启。常见的特性有：
`geometryShader`支持几何着色器、 `tessellationShader`支持曲面细分着色器、 `multiViewport`支持多个视口、 `wideLines`支持宽度大于 1 的线段。
```cpp
struct PhysicalDeviceProperties{
    Bool32 robustBufferAccess                      = {};
    Bool32 fullDrawIndexUint32                     = {};
    Bool32 imageCubeArray                          = {};
    Bool32 independentBlend                        = {};
    Bool32 geometryShader                          = {};
    Bool32 tessellationShader                      = {};
    ......
}
```
- 队列族(queueFamily)：所有绘制、计算、资源上传等操作都需要将命令(command)提交到队列(queue)去完成，不同的队列拥有不同的功能，如处理计算或者处理内存传输。
队列族就是处理相同功能的队列的集合，每个族里包含 1 到多个队列。
```cpp
struct QueueFamilyProperties {
    QueueFlags queueFlags                  = {}; // 该族支持的操作类型
    uint32_t   queueCount                  = {}; // 该族中的队列数量
    uint32_t   timestampValidBits          = {}; // 时间戳有效位数
    Extent3D   minImageTransferGranularity = {}; // 图像传输的最小粒度
 }

```
- 扩展：类似于"1. 创建instance"，一些扩展功能需要物理设备支持。`context`的扩展和物理设备的扩展不同，这里只需要一个扩展`vk::KHRSwapChainExtensionName`，用以将渲染的图像绘制到屏幕上。

最终：
```cpp
// 获取所有可用物理设备
std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
// 选取满足需求的物理设备
auto const devIter = std::ranges::find_if(
    physicalDevices, [&](auto const &physicalDeivce){
        return isDeviceSuitable(physicalDevice);
    }    
);
// 没有符合需求的物理设备
if (devIter == physicalDevices.end())
{
	throw std::runtime_error("failed to find a suitable GPU!");
}
// 选取
vk::raii::PhysicalDevice physicalDevice = *devIter;


bool isDeviceSuitable(vk::raii::PhysicalDevice const &physicalDevice){
    // 1. 设备属性检查，vulkanapi支持要大于1.3
    bool supportVulkan1_3 = physicalDevice.getProperties().apiVersion >= VK_API_VERSION_1_3；
    // 2. 设备特性检查，
    auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
                                                         vk::PhysicalDeviceVulkan11Features,
                                                         vk::PhysicalDeviceVulkan13Features,
                                                         vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool supportsRequiredFeatures = feature.tempate get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters && // 允许在着色器中直接访问 gl_DrawID 和 gl_BaseInstance/gl_BaseVertex 等内置变量，无需外部传递。对于间接绘制或实例化渲染非常有用
                                    feature.tempate get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering && // 支持动态渲染。允许你无需创建渲染通道和帧缓冲对象，直接在命令缓冲中通过 vkCmdBeginRenderingKHR / vkCmdBeginRendering 开始渲染
                                    feature.tempate get<vk::PhysicalDeviceVulkan13Features>().synchronization2 && // 提供了更完善的同步 API
                                    features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState; //它允许将更多原本需要固化到管线（Pipeline）的状态改为动态设置，例如视口、剪刀、混合常数、深度/模板比较模式等。这样可以在运行时修改它们而不用重建管线。
    // 3. 队列族检查，需要支持图形管线命令：绘制命令、渲染通道、设置图形状态、绑定管线、绑定描述符集等
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    bool supportGraphics = std::ranges::any_of(
        queueFamilies, [](auto const &qfp){
            return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
        }
    );
    // 4. 检查扩展支持
    std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName }; // 需要的扩展
    auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties(); // 物理设备支持的扩展
    bool supportsAllRequiredExtensions = std::ranges::all_of(
        requiredDeviceExtension, [&availableDeviceExtensions](auto const &requiredDeviceExtension) {
	        return std::ranges::any_of(
                availableDeviceExtensions, [requiredDeviceExtension](auto const &availableDeviceExtension) { 
                    return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0; 
                }
            );
		}
    );
    // 需要全部支持
    return supportVulkan1_3 && supportGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;

}
```
#### 3. 创建逻辑设备
在选取物理设备之后，需要创建逻辑设备`vk::raii::Device`与物理设备进行交互，可以为同一个物理设备创建多个逻辑设备。在逻辑设备中，
需要指定使用的队列族、设备特性和扩展。（在物理设备选取时只是检查，这里需要指定开启）。
```cpp
// 1. 获取第一个支持图形渲染和展示画面的队列族
uint32_t queueIndex = ~0;
std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++){
	if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
		physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
	{
		queueIndex = qfpIndex;
		break;
	}
}
if (queueIndex == ~0){
	throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
}

float queuePriority = 0.5f;
vk::DeviceQueueCreateInfo = deviceQueueCreateInfo{
    .queueFamilyIndex = queueIndex,
    .queueCount = 1,
    .pQueuePriorities = queuePriority
};

// 2. 指定要开启的feature
vk::StructureChain<vk::PhysicalDeviceFeatures2,
                  vk::PhysicalDeviceVulkan11Features,
                  vk::PhysicalDeviceVulkan13Features,
                  vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
                        {},
                        {.shaderDrawParameters = true},
                        {.synchronization2 = true, .dynamicRendering = true},
                        {.extendedDynamicState = true}
                   };

// 3. 指定要开启的扩展
std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName }; // 需要使用交换链扩展

//通过上面的信息生成DeviceCreateInfo
vk::DeviceCreateInfo deviceCreateInfo {
    .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = &deviceQueueCreateInfo,
    .enableExtensionCount = static_cast<uint2_t>(requiredDeviceExtension),
    .ppEnableExtensionNames = requiredDeviceExtension.data()
};

// 创建逻辑设备
vk::raii::Device device = vk::raii::Device(physicalDevice, deviceCreateInfo);
// 队列也要提取出来
vk::raii::Queue queue  = vk::raii::Queue(device, queueIndex, 0); // vk::raii::Queue(逻辑设备, 队列族的index, 队列的index)

```

#### 4. 创建表面(surface)
Vulkan 本身是纯图形/计算 API，不包含任何窗口系统相关的概念（如创建窗口、处理消息循环、管理屏幕输出）。
为了将渲染结果显示到屏幕上，需要额外的扩展来桥接 Vulkan 与底层操作系统的窗口管理机制。`VkSurfaceKHR`正是这座“桥梁”的核心组件。
它将不同平台的窗口（Win32、X11、Wayland、macOS/Metal、Android 等）统一成一个 Vulkan 可识别的 `VkSurfaceKHR` 对象。上层代码不需要关心具体是哪个操作系统。
`VkSwapchainKHR`（交换链）的创建必须指定一个有效的 `VkSurfaceKHR`，交换链生成的图像最终会被呈现到该表面所对应的窗口上。

这里使用glfw创建表面
```
VkSurfaceKHR _surface;
if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
	throw std::runtime_error("failed to create window surface!");
}
surface = vk::raii::SurfaceKHR(instance, _surface);
```
#### 5. 创建交换链(swap chain)
Vulkan没有“默认framebuffer”的概念，因此在我们将buffer渲染到屏幕可视化之前，需要一个基础设施来存放这个buffer，这个基础设施就是交换链(Swap Chain)。

交换链本质上是一个等待显示到屏幕上的图像队列，我们的应用程序将获取一个图像来绘制它，然后将它返回到队列。交换链可以将图像的呈现与屏幕的刷新率同步。

在创建交换链之前，我们需要检查以下三个基本属性：
- 基础的表面(surface)能力，如交换链支持的最大/最小图片数量、图片尺寸的最大最小值；

```cpp
auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR( *surface );
// 首先检查图片尺寸，单位是pixel
//
if(surfaceCapabilities.currentExtent.width != std::numeric_limits<uint_32>::max()){
    // 表明交换链有固定尺寸，表面和图片只能使用这个尺寸
    return surfaceCapabilities.currentExtent;
}
// 如果没有固定尺寸，则可以选择最大最小值之间的任意尺寸
int width, height;
glfwGetFramebufferSize(window, &width, &height);
vk::Extent2D surfaceExtent = {
    std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
    std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
};

// 设置交换链的图片数量，只设置最小图片数量
auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount); // 图片数量必须3以上，以保证使用三重缓冲
if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
{
    // 如果最数量超过了交换链允许的最大值，将最大值设置为图片数量
    minImageCount = surfaceCapabilities.maxImageCount;
}
```
- 表面格式：像素格式和色彩空间
```cpp
std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR( surface ); // 获取表面支持的格式
const auto formatIt = std::ranges::find_if(
    availableFormats, [](const auto& format){
        return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonLinear;
    }
);
vk::SurfaceFormatKHR surfaceFormat = formatIt != availabelFormats.end() ? *formatIt : availableFprmats[0]; // 如果没有满足条件的就选择第一个
```
`vk::Format::eB8G8R8A8Srgb`表示使用BGRA顺序存储色彩，每个通道的大小为8bit，一个像素32bit。
- 可用的展示模式

展示模式有以下几种：
1. `vk::PresentModeKHR::eImmediate`：应用程序提交的图像会立即被发送到显示控制器，而不等待垂直空白（Vertical Blank，VBlank）信号。显示扫描输出时，如果内存中的图像恰好被换出，就会出现撕裂。
2. `vk::PresentModeKHR::eFifo`：交换链内部维护一个队列（通常是双缓冲或三缓冲）。显示设备在每次垂直空白时，从队列前端取走一张图像显示。应用程序将渲染完成的图像推入队列后端。
若队列已满（所有图像都被占用），应用程序的`vkQueuePresentKHR`会阻塞，直到队列空出位置。
3. `vk::PresentModeKHR::eFifoRelaxed`：基本逻辑与 FIFO 相同，但有一个例外：如果队列在上一次垂直空白时为空（即应用程序来不及渲染新帧，导致显示器重复显示旧图像），
那么当新图像最终到达时，不等待下一个 VBlank，而是立即提交。这会导致部分撕裂（只有这一帧可能撕裂，后续恢复正常）。
4. `vk::PresentModeKHR::eMailbox`：交换链维护一个固定容量的队列（通常是三个缓冲区）。当队列已满时，新提交的图像不会阻塞，而是替换掉队列中最旧的一个待显示图像。
显示设备始终从队列中取走最新的图像进行显示。

**eFifo模式是保证可用的**

```cpp
std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR( surface );
vk::PresentModeKHR presentMode = 
    std::ranges::any_of(
        availablePresentModes, [](const vk::PresentModeKHR value){
            return vk::PresentModeKHR::eMailBox == value;
        }
    ) ? vk::PresentModeKHR::eMailBox : vk::PresentModeKHR::eFifo; // 首选eMailBox，没有就使用eFifo
```

最终：
```cpp
vk::SwapchainCreateInfoKHR swapChainCreateInfo{
    .surface = *surface,
    .minImageCount = minImageCount, // 图片数量
    .imageFormat = surfaceFormat.format, // 图片格式brga
    .imageColorSpace = surfaceFormat.colorSpace, // 色彩空间
    .imageExtent = surfaceExtent, // 图片尺寸
    .imageArrayLayers = 1, // 每个图像包含的层数，
    .imageUsage = vk::ImageUsageFlagBits::eColorAttachment, // 将图像作为颜色附件使用
    .imageSharingMode = vk::SharingMode::eExclusive, // 图片可能被多个队列族使用情况下怎么处理，eExclusive表示图片只能属于一个queueFamily，所有权必须显示的转移。
    .preTransform = surfaceCapabilities.currentTransform, // 图像变换操作，如旋转、水平翻转等
    .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque, // 是否使用alpha通道来与其他window进行混合
    .presentMode = presentMode, // 展示模式
    .clipped = true // true表示不关心被遮蔽的像素
}
// 创建swapchain
vk::raii::SwapchainKHR swapChain = k::raii::SwapchainKHR(device, swapChainCreateInfo);
// 保存里面的image
std::vector<vk::Image> swapChainImages = swapChain.getImages();
```
