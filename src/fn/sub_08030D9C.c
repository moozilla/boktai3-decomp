#include "global.h"

s32 Script_SeekToKeyword(s32);
u8 *Script_GetPc(void);
u32 sub_08227E90(void);
u32 sub_08030C84(u32);
u32 sub_08030D9C(void)
{
    u32 v;
    if (Script_SeekToKeyword(0x72) == 0)
        v = (u32)Script_GetPc();
    else
        v = sub_08227E90();
    return sub_08030C84(v);
}
