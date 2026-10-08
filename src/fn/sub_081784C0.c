#include "global.h"
extern u8 *gUnk_02000214;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_081784C0(void)
{
    u8 *p = gUnk_02000214;
    if (p) {
        if (Script_SeekToKeyword(0x69))
            p[0x4864] = Script_GetValue();
    }
}
