#include "global.h"

extern u8 *gUnk_02000090;

u32 sub_08017444(void) {
    if (gUnk_02000090 != 0) {
        if (gUnk_02000090[0x26] == 0) {
            return 1;
        }
    }
    return 0;
}
