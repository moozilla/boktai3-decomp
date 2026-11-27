#include "global.h"
u32 Save_GetContinuationSize(void), Save_AlignSizeToEightBytes(u32),
    Save_GetContinuationRegionBlock(u32);
void *Save_GetContinuationData(void);
s32 Save_ReadBlockWithRetries(u32, void *, u32);
u32 Save_ReadContinuation(u32 arg)
{
    u32 size = Save_AlignSizeToEightBytes(Save_GetContinuationSize());
    u32 index = Save_GetContinuationRegionBlock(arg);
    s32 result;
    index <<= 16;
    index >>= 16;
    result = Save_ReadBlockWithRetries(index, Save_GetContinuationData(), size);
    if (result < 0)
        return 0;
    return 1;
}
