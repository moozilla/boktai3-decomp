#include "global.h"

extern u8 *gUnk_02000164;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08127924(u8 *);
void sub_0821A0C0(u8 *);
void sub_08127848(void);
void sub_081278E0(void);

u8 *sub_0812793C(void)
{
    u8 *p;
    if (gUnk_02000164 != 0)
        return gUnk_02000164;
    p = sub_08219FBC(0x8, 0x864);
    if (p != 0) {
        sub_0821A04C(p, sub_08127848, sub_081278E0);
        if (sub_08127924(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
