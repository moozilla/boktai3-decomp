#include "global.h"

extern u8 *gUnk_02000208;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0811E318(u8 *);
void sub_0821A0C0(u8 *);
void sub_0811DEDC(void);
void sub_0811DEE0(void);

u8 *sub_0811E330(void)
{
    u8 *p;
    if (gUnk_02000208 != 0)
        return gUnk_02000208;
    p = sub_08219FBC(8, 0x23c);
    if (p != 0) {
        sub_0821A04C(p, sub_0811DEDC, sub_0811DEE0);
        if (sub_0811E318(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
