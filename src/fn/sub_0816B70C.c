#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
extern u8 *gUnk_02000710;
void sub_0816B70C(void)
{
    if (Script_SeekToKeyword(0x73))
        *(u16 *)(gUnk_02000710 + 0x5ac) = Script_GetValue();
}
