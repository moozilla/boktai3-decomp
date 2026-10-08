#include "global.h"
extern u8 *gUnk_0200048C;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08050F40(u8 *, u32, u32, u32);
void sub_0821A0C0(u8 *);
void sub_08050DBC(void);
void sub_08050E90(void);
u8 *sub_08051000(u32 a, u32 b, u32 c)
{
    u8 *r = gUnk_0200048C;
    if (r == 0) {
        r = sub_08219FBC(9, 0x38);
        if (r) {
            sub_0821A04C(r, sub_08050DBC, sub_08050E90);
            if (sub_08050F40(r, a, b, c) < 0) {
                sub_0821A0C0(r);
                return 0;
            }
        }
    }
    return r;
}
