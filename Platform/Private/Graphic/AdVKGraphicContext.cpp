#include "Graphic/AdVKGraphicContext.h"
#include "AdLog.h"
#include <vulkan/vulkan.h>

namespace ade {
    AdVKGraphicContext::AdVKGraphicContext() {
        CreateInstance();
    }

    AdVKGraphicContext::~AdVKGraphicContext() {

    }

    void AdVKGraphicContext::CreateInstance() {
        uint32_t availableLayerCount;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
        VkLayerProperties availableLayers[availableLayerCount];
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers);

        LOG_D("---------------------------");
        LOG_D("Instance Layers: ");
        for (int i = 0; i < availableLayerCount; i++) {
            LOG_D("\t {0}", availableLayers[i].layerName);
        }
        LOG_D("---------------------------");
    }
}