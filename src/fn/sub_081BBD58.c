#include "global.h"
void sub_0821AD08(u32, u32);
void sub_0821A0C0(u8 *);
void sub_081BBD58(u8 *p)
{
    u16 *c = (u16 *)(p + 0x70c);
    (*c)++;
    if (*c > 0x1f) {
        u32 v = *(u32 *)(p + 0x6fc);
        if (v) {
            sub_0821AD08(v, 0);
            sub_0821A0C0(p);
        }
    }
}
