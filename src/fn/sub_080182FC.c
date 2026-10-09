#include "global.h"
extern u8 *gUnk_02000090;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08018028(u8 *);
void sub_0821A0C0(u8 *);
void sub_08017F74(void);
void sub_08018014(void);
u8 *sub_080182FC(void)
{
    u8 *r;
    if (gUnk_02000090 != 0) return gUnk_02000090;
    r = sub_08219FBC(9, 0x8ec);
    if (r) {
        sub_0821A04C(r, sub_08017F74, sub_08018014);
        if (sub_08018028(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
