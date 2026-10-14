#include "global.h"
extern u8 *gUnk_030053F8;
extern u8 *gUnk_02000710;
u8 sub_0822BBA8(void);
void sub_08227DC4(void)
{
    if (gUnk_030053F8) {
        u8 r = sub_0822BBA8();
        if (r == 0) {
            u16 five = 5;
            s32 limit;
            u8 *p;
            *(u16 *)(gUnk_02000710 + 0x12) = five;
            limit = 0xe6;
            p = gUnk_030053F8;
            *(s32 *)(p + 4) = limit;
            p[8] = r;
            gUnk_030053F8[9] = r;
            gUnk_030053F8[10] = r;
        }
    }
}
