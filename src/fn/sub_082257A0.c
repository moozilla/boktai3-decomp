#include "global.h"
extern u32 gUnk_030053E8;
s32 sub_08224F2C(void);
s32 Script_SeekToKeyword(s32);
s32 sub_082257A0(void)
{
    u32 *g;
    if (sub_08224F2C() == 0) {
        g = &gUnk_030053E8;
        *g = 0x40;
        if (Script_SeekToKeyword(0x73))
            *g |= 0x10;
        else if (Script_SeekToKeyword(0x72))
            *g |= 0x100;
    }
    return 0;
}
