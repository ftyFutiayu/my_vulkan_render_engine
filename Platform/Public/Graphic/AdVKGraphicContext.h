#ifndef AD_VK_GRAPHIC_CONTEXT_H
#define AD_VK_GRAPHIC_CONTEXT_H

#include "AdGraphicContext.h"
#include "AdVKCommon.h"

namespace ade {

    struct QueueFamilyInfo {
        int32_t queueFamilyIndex = -1;
        uint32_t queueCount;
    };

    class AdVKGraphicContext : public AdGraphicContext {
    public:
        AdVKGraphicContext(AdWindow *window);

        ~AdVKGraphicContext() override;

        VkInstance GetInstance() const { return mInstance; };

        VkSurfaceKHR GetSurface() const { return mSurface; }

        VkPhysicalDevice GetPhysicalDevice() const { return mPhysicalDevice; }

        const QueueFamilyInfo &GetGraphicFamilyInfo() const { return mGraphicQueueFamily; };

        const QueueFamilyInfo &GetPresentFamilyInfo() const { return mPresentQueueFamily; };

        bool IsSameGraphicPresentQueueFamily() const {
            return mGraphicQueueFamily.queueFamilyIndex == mPresentQueueFamily.queueFamilyIndex;
        }

    private:
        void CreateInstance();

        void CreateSurface(AdWindow *window);

        void SelectPhysicalDevice();

        static void PrintPhysicalDeviceInfo(VkPhysicalDeviceProperties &properties);

        static uint32_t GetPhysicalDeviceScore(VkPhysicalDeviceProperties &properties);

    private:
        bool bShouldValidate = true;
        VkInstance mInstance;
        VkSurfaceKHR mSurface;

        // 队列族
        QueueFamilyInfo mGraphicQueueFamily{};
        QueueFamilyInfo mPresentQueueFamily{};

        // 物理设备
        VkPhysicalDevice mPhysicalDevice;
        VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
    };
}

#endif