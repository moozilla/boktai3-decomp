#include "global.h"

extern u32 *gUnk_020001A0;

void sub_08073B38(void) {
    if (gUnk_020001A0) {
        u8 *e = (u8 *)gUnk_020001A0[10];
        while (e) {
            u16 v = *(u16 *)(e + 6);
            if (v) *(u16 *)(e + 6) = v - 1;
            e = *(u8 **)(e + 0xc);
        }
    }
}
