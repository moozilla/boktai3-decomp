#define Save_WriteBlockWithRetries sub_0822F0A4
#define Save_AlignBlockSize Save_AlignSizeToEightBytes
#define Save_BeginTransfer Save_SuspendRuntimeForTransfer
#define Save_EndTransfer Save_ResumeRuntimeAfterTransfer
#define Save_WriteBlock sub_0822EE68
#include "global.h"
u32 Save_AlignSizeToEightBytes(u32);
u32 Save_SuspendRuntimeForTransfer(void);
u32 Save_ResumeRuntimeAfterTransfer(void);
s32 sub_0822EE68(u16, void *, s32);
s32 sub_0822F0A4(u16 index, void *buffer, s32 size)
{
    s32 result, i;
    if (((u32)buffer & 1) || size != Save_AlignSizeToEightBytes(size) || size > 0x1ff8)
        return -1;
    Save_SuspendRuntimeForTransfer();
    result = -1;
    for (i = 0; i < 8; i++) {
        result = sub_0822EE68(index, buffer, size);
        if (result >= 0)
            break;
    }
    Save_ResumeRuntimeAfterTransfer();
    return result;
}
