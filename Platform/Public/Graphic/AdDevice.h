//
// Created by 12381 on 2024/11/18.
//

#ifndef AD_VK_DEVICE_H
#define AD_VK_DEVICE_H

#include "AdVKCommon.h"

namespace ade {
    class AdVKGraphicContext;

    class AdVKQueue;

    struct AdVkSettings {

    };

    class AdVKDevice {
    public:
        AdVKDevice(AdVKGraphicContext *context, uint32_t graphicQueueCount, uint32_t presentQueueCount,
                   const AdVkSettings &settings = {});

        ~AdVKDevice();

    private:
        VkDevice mDevice;

        std::vector<std::shared_ptr<AdVKQueue>> mGraphicQueues;
        std::vector<std::shared_ptr<AdVKQueue>> mPresentQueues;
    };
}


#endif
