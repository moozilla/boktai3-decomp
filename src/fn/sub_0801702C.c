#include "global.h"

extern u32 gUnk_02000078;
u32 sub_08016FE0(void);

u32 sub_0801702C(void) {
    u32 r = gUnk_02000078;
    if (r == 0) {
        return sub_08016FE0();
    }
    return r;
}
