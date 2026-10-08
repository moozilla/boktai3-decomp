#include "global.h"

extern u8 *gUnk_02000030;

u32 sub_08006FAC(u8 *p) {
    u32 *slot;
    u8 *t;
    *(u32 *)(p + 0xB4) = 0;
    slot = (u32 *)(p + 0xB8);
    *slot = *(u32 *)(gUnk_02000030 + 0x28);
    t = (u8 *)*slot;
    if (t) {
        *(u8 **)(t + 0xB4) = p;
    }
    *(u8 **)(gUnk_02000030 + 0x28) = p;
    return 0;
}
