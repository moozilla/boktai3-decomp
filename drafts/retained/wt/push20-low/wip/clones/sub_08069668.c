#include "global.h"

extern u8 *gUnk_0200014C;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08069620(u8 *);
void sub_0821A0C0(u8 *);
void sub_08069510(void);
void sub_080695C8(void);

u8 *sub_08069668(void)
{
    u8 *p;
    if (gUnk_0200014C != 0)
        return gUnk_0200014C;
    p = sub_08219FBC(0x8, 0x19c);
    if (p != 0) {
        sub_0821A04C(p, sub_08069510, sub_080695C8);
        if (sub_08069620(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
