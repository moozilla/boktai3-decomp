#include "global.h"

struct P {
    u8 f[0x2c];
    u32 flags;
};
extern struct P *gUnk_020004A8;
u32 Script_GetKeywordValue(s32, s32);

void sub_080727EC(void)
{
    struct P *p = gUnk_020004A8;
    if (p != 0)
        p->flags &= ~(u16)Script_GetKeywordValue(0x73, 0);
}
