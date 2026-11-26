#include "global.h"
s32 Save_AlignSizeToEightBytes(s32);
void Save_SuspendRuntimeForTransfer(void);
s32 Save_VerifyBlock(u16, s32);
void Save_ResumeRuntimeAfterTransfer(void);

s32 sub_0822F174(u16 block, s32 size)
{
    s32 result;
    if (size != Save_AlignSizeToEightBytes(size) || size > 0x1ff8)
        return -1;
    Save_SuspendRuntimeForTransfer();
    result = Save_VerifyBlock(block, size);
    Save_ResumeRuntimeAfterTransfer();
    return result;
}
