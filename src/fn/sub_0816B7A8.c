#include "global.h"
extern u8 *gUnk_02000710;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0816B7A8(void)
{
    if (Script_SeekToKeyword(0x69)) {
        s32 v = Script_GetValue();
        *(u32 *)(gUnk_02000710 + 0x5B8) |= 1 << v;
    }
}
