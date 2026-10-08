#include "global.h"
extern u16 gUnk_02000590;
extern u8 *gUnk_02000710;
s32 sub_0804DF4C(void *);
s32 sub_08179FF8(u8 *p) {
    s32 r;
    if (gUnk_02000590 != 0) {
        r = 4;
    } else if (sub_0804DF4C(p + 0x54) == 0) {
        r = 0;
    } else {
        r = *(s16 *)(gUnk_02000710 + 0x876);
    }
    return r;
}
