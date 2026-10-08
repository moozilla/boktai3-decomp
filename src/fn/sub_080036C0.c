#include "global.h"

extern u8 *gUnk_02000468;

u32 sub_080036C0(void) {
    return gUnk_02000468 ? gUnk_02000468[0x4C] : 0x40;
}
