#include "global.h"
struct P { u8 f0[0x342]; u8 e; };
extern u32 gUnk_02000210;
void sub_08214514(u8 *);
void sub_082195E0(u8 *);
u32 sub_081636C0(u8 *p)
{
    u8 *q;
    s32 i;
    if (((struct P *)p)->e != 0) {
        q = p + 0x18;
        for (i = 7; i >= 0; i--) {
            sub_08214514(q);
            q += 0x50;
        }
        sub_082195E0(p + 0x2b8);
    }
    return gUnk_02000210 = 0;
}
