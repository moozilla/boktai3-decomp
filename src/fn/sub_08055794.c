#include "global.h"
u8 *sub_08055678(void);
s32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
void sub_08055794(void)
{
    u8 *p = sub_08055678();
    if (p) {
        if (Script_SeekToKeyword(0x70))
            p[0x52] = Script_GetValue();
    }
}
