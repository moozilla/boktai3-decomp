#include "global.h"
extern u8 *gUnk_020000B0;
u8 *sub_08019AE4(void);
u8 *sub_08019B30(u8 *);
void sub_0821456C(u8 *);
u8 *sub_08019B7C(void)
{
    u8 *s = gUnk_020000B0;
    u8 *q;
    if (s != 0 || (s = sub_08019AE4()) != 0) {
        q = sub_08019B30(s);
        if (q != 0) {
            sub_0821456C(q + 0xc);
            return q;
        }
    }
    return 0;
}
