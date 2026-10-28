#include "global.h"
#include "runtime.h"
u32 Save_AlignSizeToEightBytes(u32);
u32 Save_WriteBlockWithRetries(u32, u32, u32);
u32 Save_WriteHeader(void)
{
    u32 n = Save_AlignSizeToEightBytes(0x28);
    if (Save_WriteBlockWithRetries(0, gSessionState, n) != n + 8)
        return 0;
    return 1;
}
