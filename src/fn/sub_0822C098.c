#include "global.h"
#include "ram.h"
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
u32 Region_InitFromScript(u32 *p)
{
    if (Script_SeekToKeyword(0x6d)) {
        u32 *q = p + 6;
        while (Script_GetPc())
            *q++ = Script_GetValue();
    }
    gRegionList = (u8 *)p;
    return 0;
}
