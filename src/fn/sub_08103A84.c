#include "global.h"

extern u8 *gUnk_020001AC;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08103A54(u8 *);
void sub_0821A0C0(u8 *);
void sub_08103850(void);
void sub_0810399C(void);

u8 *sub_08103A84(void)
{
    u8 *p;
    if (gUnk_020001AC != 0)
        return gUnk_020001AC;
    p = sub_08219FBC(5, 0x137c);
    if (p != 0) {
        sub_0821A04C(p, sub_08103850, sub_0810399C);
        if (sub_08103A54(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
