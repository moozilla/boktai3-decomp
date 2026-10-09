#include "global.h"
extern u8 *gUnk_02000078;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08016F24(u8 *);
void sub_0821A0C0(u8 *);
void sub_08016EA8(void);
void sub_08016EFC(void);
u8 *sub_08016FE0(void)
{
    u8 *r;
    if (gUnk_02000078 != 0) return gUnk_02000078;
    r = sub_08219FBC(9, 0xef8);
    if (r) {
        sub_0821A04C(r, sub_08016EA8, sub_08016EFC);
        if (sub_08016F24(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
