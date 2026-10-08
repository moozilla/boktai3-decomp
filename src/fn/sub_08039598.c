#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08039568(u8 *);
void sub_0821A0C0(u8 *);
void sub_080394D4(void);
void sub_08039548(void);

u8 *sub_08039598(void)
{
    u8 *p;
    p = sub_08219FBC(3, 0x20);
    if (p != 0) {
        sub_0821A04C(p, sub_080394D4, sub_08039548);
        if (sub_08039568(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
