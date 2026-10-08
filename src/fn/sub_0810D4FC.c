#include "global.h"

extern u8 *gUnk_020001C8;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0810D3A0(u8 *);
void sub_0821A0C0(u8 *);
void sub_0810D334(void);
void sub_0810D36C(void);

u8 *sub_0810D4FC(void)
{
    u8 *p;
    if (gUnk_020001C8 != 0)
        return gUnk_020001C8;
    p = sub_08219FBC(0xb, 0x1414);
    if (p != 0) {
        sub_0821A04C(p, sub_0810D334, sub_0810D36C);
        if (sub_0810D3A0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
