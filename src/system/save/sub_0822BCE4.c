#include "global.h"
u32 Save_GetContinuationSize(void), Save_AlignSizeToEightBytes(u32),
    Save_GetContinuationRegionBlock(u32);
void *Save_GetContinuationSnapshot(void);
s32 Save_WriteBlockWithRetries(u32, void *, u32);
u32 Save_WriteContinuation(u32 arg)
{
    u32 size = Save_AlignSizeToEightBytes(Save_GetContinuationSize());
    u32 index = Save_GetContinuationRegionBlock(arg);
    s32 result;
    index <<= 16;
    index >>= 16;
    result = Save_WriteBlockWithRetries(index, Save_GetContinuationSnapshot(), size);
    if (result != size + 8)
        return 0;
    return 1;
}
