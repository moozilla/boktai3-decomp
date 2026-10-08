#include "global.h"
struct P { u32 a, b, c, d; };
struct Q { u8 f[0x18]; struct P *p; };
extern struct Q *gUnk_020004B8;
u8 sub_08110B7C(void);
void sub_0811119C(struct P *dst)
{
    if (sub_08110B7C()) {
        *dst = *gUnk_020004B8->p;
    }
}
