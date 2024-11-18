//
// Created by 12381 on 2024/11/18.
//

#ifndef AD_VK_QUEUE_H
#define AD_VK_QUEUE_H

#include "AdVKCommon.h"

namespace ade{
    class AdVKQueue{
    public:
        AdVKQueue(uint32_t familyIndex, uint32_t index, VkQueue queue, bool canPresent);
        ~AdVKQueue() = default;

        void WaitIdle() const;
    private:
        uint32_t mFamilyIndex;
        uint32_t mIndex;
        VkQueue mQueue;
        bool canPresent;
    };
}

#endif
