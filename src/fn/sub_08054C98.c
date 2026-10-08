#include "global.h"
u8 *sub_08054B80(void);
s32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
void sub_08054C98(void)
{
    u8 *p = sub_08054B80();
    if (p) {
        if (Script_SeekToKeyword(0x70))
            p[0x2F] = Script_GetValue();
    }
}
