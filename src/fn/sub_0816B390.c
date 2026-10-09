#include "global.h"
extern u8 *gUnk_02000710;
extern u16 gUnk_03004BD8;
u32 sub_0816AB98(void);
void sub_0816AC64(void *);
void sub_0816B390(u8 *s) {
    s[0x48E6] = sub_0816AB98();
    {
        u16 *p = &gUnk_03004BD8;
        u32 mask = ~0x200;
        *p &= mask;
    }
    sub_0816AC64(s);
}
