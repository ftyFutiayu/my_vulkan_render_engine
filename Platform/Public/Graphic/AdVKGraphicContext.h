#ifndef AD_VK_GRAPHIC_CONTEXT_H
#define AD_VK_GRAPHIC_CONTEXT_H

#include "AdGraphicContext.h"

namespace ade {
    class AdVKGraphicContext : public AdGraphicContext {
    public:
        AdVKGraphicContext();

        ~AdVKGraphicContext() override;

    private:
        void CreateInstance();
    };
}

#endif