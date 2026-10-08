#include "global.h"

extern u8 *gUnk_02000484;

u16 sub_08036C88(void) {
    u16 r;
    if (gUnk_02000484) r = *(u16 *)(gUnk_02000484 + 0x6C);
    else r = 0;
    return r;
}
