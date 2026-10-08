#include "global.h"

extern u8 *gUnk_02000468;

void sub_0800347C(u32 v) {
    if (gUnk_02000468) {
        gUnk_02000468[0x1A] = v;
    }
}
