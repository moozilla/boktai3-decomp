#include "global.h"

s32 Script_GetValue(void);
u8 *sub_08070EBC(void);

s32 sub_08070EE4(void)
{
    u8 *p;
    s32 r;
    s32 v;
    if (Script_GetValue() == 0 || (p = sub_08070EBC()) == 0)
        return 0;
    v = *(s16 *)(p + 0x9c);
    r = 1;
    if (v <= 0)
        r = 0;
    return r;
}
