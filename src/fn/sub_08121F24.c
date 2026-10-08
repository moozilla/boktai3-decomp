#include "global.h"

extern u8 *gUnk_02000138;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08121F0C(u8 *);
void sub_0821A0C0(u8 *);
void sub_08121E28(void);
void sub_08121E90(void);

u8 *sub_08121F24(void)
{
    u8 *p;
    if (gUnk_02000138 != 0)
        return gUnk_02000138;
    p = sub_08219FBC(0xa, 0x27c);
    if (p != 0) {
        sub_0821A04C(p, sub_08121E28, sub_08121E90);
        if (sub_08121F0C(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
