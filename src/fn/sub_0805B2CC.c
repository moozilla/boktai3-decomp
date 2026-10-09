#include "global.h"
extern u8 *gUnk_02000488;
extern u8 *gUnk_02000580;
s32 sub_0805B28C(void *, void *, s32);
s32 sub_0805B2CC(u8 *p) {
    u8 *a = gUnk_02000488;
    if (a && *(u32 *)(gUnk_02000580 + 0x1c) == 1 && gUnk_02000580[0x457] == 6 && sub_0805B28C(a + 0x50, p + 0x510, 0x46))
        return 1;
    return 0;
}
