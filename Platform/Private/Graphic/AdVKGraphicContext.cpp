#include "Graphic/AdVKGraphicContext.h"
#include "Window/AdGLFWwindow.h"

namespace ade {

    // 必须的扩展和层级
    const DeviceFeature requestedLayers[] = {
            {"VK_LAYER_KHRONOS_validation", true},
    };

    const DeviceFeature requestedExtensions[] = {
            {VK_KHR_SURFACE_EXTENSION_NAME, true},
            {"VK_EXT_debug_utils", false},
            {"VK_EXT_debug_report", true},
#ifdef AD_ENGINE_PLATFORM_WIN32
            {VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true},
#elif AD_ENGINE_PLATFORM_MACOS
            { VK_MVK_MACOS_SURFACE_EXTENSION_NAME, true },
#elif AD_ENGINE_PLATFORM_LINUX
            { VK_KHR_XCB_SURFACE_EXTENSION_NAME, true },
#endif
    };


    AdVKGraphicContext::AdVKGraphicContext(AdWindow *window) {
        CreateInstance();

        CreateSurface(window);

        SelectPhysicalDevice();
    }

    AdVKGraphicContext::~AdVKGraphicContext() {
        vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

        vkDestroyInstance(mInstance, nullptr);
    }

    // Vulkan 验证层日志回调
    static VkBool32 VkDebugReportVCallback(VkDebugReportFlagsEXT flags,
                                           VkDebugReportObjectTypeEXT objectType,
                                           uint64_t object,
                                           size_t location,
                                           int32_t messageCode,
                                           const char *pLayerPrefix,
                                           const char *pMessage,
                                           void *pUserData) {
        // 打印 Error和Warn级别日志
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
            LOG_E("{0}", pMessage);
        }
        if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
            LOG_W("{0}", pMessage);
        }
        return VK_TRUE;
    }

    // 创建Vulkan 实例
    void AdVKGraphicContext::CreateInstance() {
        // 1. 查询支持的 layer并构建
        uint32_t availableLayerCount;
        CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));

        VkLayerProperties availableLayers[availableLayerCount];

        CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers));

        uint32_t enableLayerCount = 0;
        const char *enableLayers[32];

        if (bShouldValidate) {
            // 判断是否支持需要的层
            if (!checkDeviceFeatures("Instance Layers", false, availableLayerCount, availableLayers,
                                     ARRAY_SIZE(requestedLayers), requestedLayers,
                                     &enableLayerCount, enableLayers)) {
                return;
            }
        }

        // 2. 查询支持的扩展并且构建

        uint32_t availableExtensionsCount;
        CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionsCount, nullptr));
        VkExtensionProperties availableExtensions[availableExtensionsCount];
        CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionsCount, availableExtensions));

        uint32_t glfwRequestedExtensionsCount;
        // 获取 GLFW 所需要的扩展的 字符串指针的指针
        const char **glfwRequestedExtensions = glfwGetRequiredInstanceExtensions(&glfwRequestedExtensionsCount);
        std::unordered_set<const char *> allRequestedExtensionSet;
        std::vector<DeviceFeature> allRequestedExtensions;
        for (const auto &item: requestedExtensions) {
            if (allRequestedExtensionSet.find(item.name) == allRequestedExtensionSet.end()) {
                allRequestedExtensionSet.insert(item.name);
                allRequestedExtensions.push_back(item);
            }
        }
        for (int i = 0; i < glfwRequestedExtensionsCount; i++) {
            const char *extensionName = glfwRequestedExtensions[i];
            if (allRequestedExtensionSet.find(extensionName) == allRequestedExtensionSet.end()) {
                allRequestedExtensionSet.insert(extensionName);
                allRequestedExtensions.push_back({extensionName, true});
            }
        }

        uint32_t enableExtensionCount;
        const char *enableExtensions[32];
        if (!checkDeviceFeatures("Instance Extension", true, availableExtensionsCount, availableExtensions,
                                 allRequestedExtensions.size(), allRequestedExtensions.data(),
                                 &enableExtensionCount, enableExtensions)) {
            return;
        }

        // 3. 创建 Vulkan 实例
        VkApplicationInfo applicationInfo = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext = nullptr,
                .pApplicationName = "My_Vulkan_Engine",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = "None",
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = VK_API_VERSION_1_3
        };

        VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfoExt = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
                .pNext = nullptr,
                .flags = VK_DEBUG_REPORT_WARNING_BIT_EXT
                         | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
                         | VK_DEBUG_REPORT_ERROR_BIT_EXT,
                .pfnCallback = VkDebugReportVCallback
        };

        VkInstanceCreateInfo instanceCI{
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = bShouldValidate ? &debugReportCallbackInfoExt : nullptr,
                .pApplicationInfo = &applicationInfo,
                .enabledLayerCount = enableLayerCount,
                .ppEnabledLayerNames = enableLayerCount > 0 ? enableLayers : nullptr,
                .enabledExtensionCount = enableExtensionCount,
                .ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr,
        };


        CALL_VK(vkCreateInstance(&instanceCI, nullptr, &mInstance));
        LOG_T("{0} : instance : {1}", __FUNCTION__, (void *) mInstance);
    }

    void AdVKGraphicContext::CreateSurface(ade::AdWindow *window) {
        if (!window) {
            LOG_E("window is not exists.");
            return;
        }
        auto *glfwWindow = dynamic_cast<AdGLFWwindow *>(window);

        if (!glfwWindow) {
            LOG_E("this window is not a glfw window.");
            return;
        }
        CALL_VK(glfwCreateWindowSurface(mInstance, glfwWindow->GetWindowHandle(), nullptr, &mSurface));
        LOG_T("{0} : surface : {1}", __FUNCTION__, (void *) mSurface);
    }

    // 渲染物理设备
    void AdVKGraphicContext::SelectPhysicalDevice() {
        uint32_t physicalDeviceCount = 0;
        CALL_VK(vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr));
        VkPhysicalDevice physicalDevice[physicalDeviceCount];
        CALL_VK(vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, physicalDevice));

        // 物理设备打分
        uint32_t maxScore = 0;
        int32_t maxScorePhysicalDeviceIndex = -1;

        LOG_D("-----------------------------");
        LOG_D("Physical devices: ");

        for (int i = 0; i < physicalDeviceCount; i++) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevice[i], &properties);
            PrintPhysicalDeviceInfo(properties);
            auto score = GetPhysicalDeviceScore(properties);

            uint32_t formatCount;
            CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice[i], mSurface, &formatCount, nullptr));
            VkSurfaceFormatKHR surfaceFormat[formatCount];
            CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice[i], mSurface, &formatCount, surfaceFormat));
            for (int j = 0; j < formatCount; j++) {
                if (surfaceFormat[j].format == VK_FORMAT_B8G8R8A8_UNORM &&
                    surfaceFormat[j].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
                    score += 10;
                    break;
                }
            }

            // 查询队列族
            uint32_t queueCount;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice[i], &queueCount, nullptr);
            VkQueueFamilyProperties queueFamilyProperties[queueCount];
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice[i], &queueCount, queueFamilyProperties);
            LOG_D("score    --->    : {0}", score);
            LOG_D("queue family     : {0}", queueCount);

            if (score < maxScore) {
                continue;
            }

            for (int j = 0; j < queueCount; j++) {
                if (queueFamilyProperties[j].queueCount == 0) {
                    continue;
                }
                // 找到显示和图形队列
                // 1. Graphics
                if (queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    mGraphicQueueFamily.queueCount = queueFamilyProperties[j].queueCount;
                    mGraphicQueueFamily.queueFamilyIndex = j;
                }

                // 如果找到不一样的图形和显示队列，直接退出循环
                if (mGraphicQueueFamily.queueFamilyIndex >= 0 && mPresentQueueFamily.queueFamilyIndex >= 0
                    && mGraphicQueueFamily.queueFamilyIndex != mPresentQueueFamily.queueFamilyIndex) {
                    break;
                }

                // 2. Present
                VkBool32 bSuppportSurface;
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice[i], j, mSurface, &bSuppportSurface);
                if (bSuppportSurface) {
                    mPresentQueueFamily.queueCount = queueFamilyProperties[j].queueCount;
                    mPresentQueueFamily.queueFamilyIndex = j;
                }
            }

            if (mPresentQueueFamily.queueFamilyIndex >= 0 && mPresentQueueFamily.queueFamilyIndex >= 0) {
                maxScorePhysicalDeviceIndex = i;
                maxScore = score;
            }
        }
        LOG_D("-----------------------------");


        if (maxScorePhysicalDeviceIndex < 0) {
            LOG_W("May can not fine a suitable physical device");
            maxScorePhysicalDeviceIndex = 0;
        }

        mPhysicalDevice = physicalDevice[maxScorePhysicalDeviceIndex];

        // 查询物理设备内存属性
        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mPhysicalDeviceMemoryProperties);

        LOG_T("{0} : physical device:{1}, score:{2}, graphic queue: {3} : {4}, present queue: {5} : {6}", __FUNCTION__,
              maxScorePhysicalDeviceIndex, maxScore,
              mGraphicQueueFamily.queueFamilyIndex, mGraphicQueueFamily.queueCount,
              mPresentQueueFamily.queueFamilyIndex, mPresentQueueFamily.queueCount);
    }

    void AdVKGraphicContext::PrintPhysicalDeviceInfo(VkPhysicalDeviceProperties &properties) {
        const char *deviceType = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "integrated gpu" :
                                 properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "discrete gpu" :
                                 properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ? "virtual gpu" :
                                 properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ? "cpu" : "others";

        uint32_t driverVersionMajor = VK_VERSION_MAJOR(properties.driverVersion);
        uint32_t driverVersionMinor = VK_VERSION_MINOR(properties.driverVersion);
        uint32_t driverVersionPatch = VK_VERSION_PATCH(properties.driverVersion);

        uint32_t apiVersionMajor = VK_VERSION_MAJOR(properties.apiVersion);
        uint32_t apiVersionMinor = VK_VERSION_MINOR(properties.apiVersion);
        uint32_t apiVersionPatch = VK_VERSION_PATCH(properties.apiVersion);

        LOG_D("-----------------------------");
        LOG_D("deviceName       : {0}", properties.deviceName);
        LOG_D("deviceType       : {0}", deviceType);
        LOG_D("vendorID         : {0}", properties.vendorID);
        LOG_D("deviceID         : {0}", properties.deviceID);
        LOG_D("driverVersion    : {0}.{1}.{2}", driverVersionMajor, driverVersionMinor, driverVersionPatch);
        LOG_D("apiVersion       : {0}.{1}.{2}", apiVersionMajor, apiVersionMinor, apiVersionPatch);
    }

    uint32_t AdVKGraphicContext::GetPhysicalDeviceScore(VkPhysicalDeviceProperties &properties) {
        VkPhysicalDeviceType deviceType = properties.deviceType;
        uint32_t score = 0;
        // 设备类型
        switch (deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                score += 50;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                score += 40;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                score += 20;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                score += 10;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
                break;
        }
        return score;
    }
}