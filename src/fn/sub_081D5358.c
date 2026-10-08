#include "global.h"

extern u8 *gUnk_02000710;

u32 sub_081D5358(void) {
    u8 *base = gUnk_02000710;
    if (*(s32 *)(base + 0x6F0) == -1) {
        if (*(s32 *)(base + 0x6F4) == -1) {
            if ((*(u32 *)(base + 0x6F8) & 0x7FFFF) == 0x7FFFF) {
                return 1;
            }
        }
    }
    return 0;
}
