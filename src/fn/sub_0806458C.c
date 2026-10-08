#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0806454C(u8 *, u32, u32);
void sub_0821A0C0(u8 *);
void sub_080642EC(void);
void sub_08064314(void);
u8 *sub_0806458C(u32 a, u32 b)
{
    u8 *r = sub_08219FBC(8, 0x144);
    if (r) {
        sub_0821A04C(r, (void (*)(void))sub_080642EC, (void (*)(void))sub_08064314);
        if (sub_0806454C(r, a, b) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
