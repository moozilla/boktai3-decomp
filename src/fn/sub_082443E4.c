#include "global.h"
struct X { u8 f[4]; u8 a; u8 g[0x1f]; u32 *b; };
extern struct X *gUnk_03006A50;
s32 sub_08244834(u32);
void sub_0824490C(void);
void sub_082443E4(u16 v)
{
    if ((s16)sub_08244834(0x1f) == 0) {
        gUnk_03006A50->a = 1;
        gUnk_03006A50->b[1] = v;
        sub_0824490C();
    }
}
