#include "global.h"

extern u8 *gUnk_0200015C;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08126CC0(u8 *);
void sub_0821A0C0(u8 *);
void sub_08126C3C(void);
void sub_08126C84(void);

u8 *sub_08126CE0(void)
{
    u8 *p;
    if (gUnk_0200015C != 0)
        return gUnk_0200015C;
    p = sub_08219FBC(0xa, 0x2038);
    if (p != 0) {
        sub_0821A04C(p, sub_08126C3C, sub_08126C84);
        if (sub_08126CC0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
