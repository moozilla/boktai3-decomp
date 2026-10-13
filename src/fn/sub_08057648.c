#include "global.h"
struct P {
    u8 f[0x18];
    u16 f18;
    u16 f1a;
};
extern struct P *gUnk_0200049C;
s32 Script_GetKeywordValue(u32, u32);
void sub_08057648(void)
{
    struct P *p = gUnk_0200049C;
    if (p != 0) {
        if (p->f1a == 0) {
            if (p->f18 != 0)
                p->f18 += Script_GetKeywordValue(0x74, 0);
        }
    }
}
