#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
extern u8 *gUnk_02000710;
void sub_0816B780(void)
{
    if (Script_SeekToKeyword(0x66))
        *(u16 *)(gUnk_02000710 + 0x5b2) = Script_GetValue();
}
