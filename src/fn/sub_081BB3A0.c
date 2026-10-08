#include "global.h"
extern u8 *gUnk_02000710;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_081BB3A0(void)
{
    if (Script_SeekToKeyword(0x61)) {
        s32 v = Script_GetValue();
        if (v > 9999) v = 9999;
        *(u16 *)(gUnk_02000710 + 0x7b0) = v;
    }
}
