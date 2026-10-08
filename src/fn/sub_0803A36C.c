#include "global.h"
struct P { u8 pad[0x18]; u8 b18; u8 b19; u8 b1a; u8 b1b; };
s32 sub_08199790(void);
s32 sub_081998B0(void);
s32 sub_081999CC(void);
s32 sub_08028E6C(u32);
void sub_0803A36C(struct P *p)
{
    if (p->b1a != 0) {
        if (sub_08199790() != 0) p->b1a--;
    }
    if (p->b19 != 0) {
        if (sub_081998B0() != 0) p->b19--;
    }
    if (p->b1b != 0) {
        if (sub_081999CC() != 0) p->b1b--;
    }
    if (p->b18 != 0) {
        if (sub_08028E6C(0xb4) != 0) p->b18--;
    }
}
