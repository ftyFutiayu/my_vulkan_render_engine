#ifndef AD_GRAPHIC_CONTEXT_H
#define AD_GRAPHIC_CONTEXT_H

#include "AdEngine.h"

namespace ade {
    class AdGraphicContext {
    public:
        AdGraphicContext(const AdGraphicContext &) = delete;

        AdGraphicContext &operator=(const AdGraphicContext &) = delete;

        virtual ~AdGraphicContext() = default;

        static std::unique_ptr<AdGraphicContext> Create();

    protected:
        AdGraphicContext() = default;

    private:

    };
}

#endif