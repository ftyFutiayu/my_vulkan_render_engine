//
// Created by 12381 on 2024/11/18.
//

#include "Graphic/AdDevice.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVkQueue.h"

using namespace ade;

const DeviceFeature requestedExtensions[] = {
        {VK_KHR_SWAPCHAIN_EXTENSION_NAME, true},
#ifdef AD_ENGINE_PLATFORM_WIN32
#elif AD_ENGINE_PLATFORM_MACOS
        { "VK_KHR_portability_subset", true },
#elif AD_ENGINE_PLATFORM_LINUX
#endif
};

AdVKDevice::AdVKDevice(AdVKGraphicContext *context,
                       uint32_t graphicQueueCount,
                       uint32_t presentQueueCount,
                       const ade::AdVkSettings &settings) {
    if (!context) {
        LOG_E("Must create a vulkan graphic context before create device.");
        return;
    }

    QueueFamilyInfo graphicQueueFamilyInfo = context->GetGraphicFamilyInfo();
    QueueFamilyInfo presentQueueFamilyInfo = context->GetPresentFamilyInfo();

    // 判断队列数量是否满足要求
    if (graphicQueueCount > graphicQueueFamilyInfo.queueCount) {
        LOG_E("this queue family has {0} queue, but request {1}", graphicQueueFamilyInfo.queueCount, graphicQueueCount);
        return;
    }
    if (presentQueueCount > presentQueueFamilyInfo.queueCount) {
        LOG_E("this queue family has {0} queue, but request {1}", presentQueueFamilyInfo.queueCount, presentQueueCount);
        return;
    }

    // --------------- 1.构建队列信息 ---------------
    std::vector<float> graphicQueuePriorities(graphicQueueCount, 0.f);
    std::vector<float> presentQueuePriorities(graphicQueueCount, 1.f);

    bool bSameQueueFamilyIndex = context->IsSameGraphicPresentQueueFamily();
    uint32_t sameQueueCount = graphicQueueCount;

    // 如果显示队列和图形队列为同一个，需要加上显示队列所需要的数量
    if (bSameQueueFamilyIndex) {
        sameQueueCount += presentQueueCount;
        if (sameQueueCount > graphicQueueFamilyInfo.queueCount) {
            sameQueueCount = graphicQueueFamilyInfo.queueCount;
        }
        graphicQueuePriorities.insert(graphicQueuePriorities.end(), presentQueuePriorities.begin(),
                                      presentQueuePriorities.end());
    }

    VkDeviceQueueCreateInfo queueInfos[2] = {
            {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .queueFamilyIndex = static_cast<uint32_t>(graphicQueueFamilyInfo.queueFamilyIndex),
                    .queueCount = sameQueueCount,
                    .pQueuePriorities = graphicQueuePriorities.data()
            }
    };
    if (!bSameQueueFamilyIndex) {
        queueInfos[1] = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = static_cast<uint32_t>(presentQueueFamilyInfo.queueFamilyIndex),
                .queueCount = presentQueueCount,
                .pQueuePriorities = presentQueuePriorities.data()
        };
    }

    // --------------- 2.逻辑设备扩展 ---------------
    uint32_t availableExtensionCount;
    CALL_VK(vkEnumerateDeviceExtensionProperties(context->GetPhysicalDevice(), "",
                                                 &availableExtensionCount, nullptr));
    VkExtensionProperties availableExtensions[availableExtensionCount];
    CALL_VK(vkEnumerateDeviceExtensionProperties(context->GetPhysicalDevice(), "",
                                                 &availableExtensionCount, availableExtensions));
    uint32_t enableExtensionCount;
    const char *enableExtensions[32];

    if (!checkDeviceFeatures("Device Extension", true, availableExtensionCount, availableExtensions,
                             ARRAY_SIZE(requestedExtensions), requestedExtensions, &enableExtensionCount,
                             enableExtensions)) {
        return;
    }

    // --------------- 3.创建逻辑设备 ---------------
    VkDeviceCreateInfo deviceCI = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = static_cast<uint32_t>(bSameQueueFamilyIndex ? 1 : 2),
            .pQueueCreateInfos = queueInfos,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = enableExtensionCount,
            .ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr,
            .pEnabledFeatures = nullptr
    };
    CALL_VK(vkCreateDevice(context->GetPhysicalDevice(), &deviceCI, nullptr, &mDevice));
    LOG_T("VkDevice: {0}", (void *) mDevice);

    for (int i = 0; i < graphicQueueCount; i++) {
        VkQueue queue;
        vkGetDeviceQueue(mDevice, graphicQueueFamilyInfo.queueFamilyIndex, i, &queue);
        mGraphicQueues.push_back(std::make_shared<AdVKQueue>(graphicQueueFamilyInfo.queueFamilyIndex, i, queue, false));
    }
    for (int i = 0; i < presentQueueCount; i++) {
        VkQueue queue;
        vkGetDeviceQueue(mDevice, presentQueueFamilyInfo.queueFamilyIndex, i, &queue);
        mPresentQueues.push_back(std::make_shared<AdVKQueue>(presentQueueFamilyInfo.queueFamilyIndex, i, queue, true));
    }
}


