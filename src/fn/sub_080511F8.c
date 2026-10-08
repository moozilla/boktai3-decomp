#include "global.h"
extern u8 *gUnk_02000114;
extern u16 gUnk_0200052C;
s32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
void sub_080511F8(void)
{
    if (gUnk_02000114) {
        if (Script_SeekToKeyword(0x48))
            gUnk_0200052C = Script_GetValue();
    }
}
