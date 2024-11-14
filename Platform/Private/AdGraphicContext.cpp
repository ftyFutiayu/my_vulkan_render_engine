#include "AdGraphicContext.h"
#include "Graphic/AdVKGraphicContext.h"

namespace ade {
    std::unique_ptr<AdGraphicContext> AdGraphicContext::Create() {
#ifdef AD_ENGINE_GRAPHIC_API_VULKAN
        return std::make_unique<AdVKGraphicContext>();
#endif
        return nullptr;
    }
}