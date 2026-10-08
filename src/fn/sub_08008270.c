#include "global.h"

extern u8 *gUnk_02000030;
u32 sub_08008230(u32 a, u32 b);

u32 sub_08008270(void) {
    if (gUnk_02000030 == 0) {
        return sub_08008230(0, 0);
    }
    return (u32)gUnk_02000030;
}
