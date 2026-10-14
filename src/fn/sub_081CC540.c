#include "global.h"
static inline void orr(u16 *a, u32 m)
{
    *a = m | *a;
}
/* Blink for 15 frames: bit 0 of the phase sets display flag 0x100, and each
 * phase lasts one frame longer than the last. */
void sub_081CC540(u8 *a, u8 *b)
{
    u32 t;
    u8 phase;
    if (b[9]) {
        u32 one;
        b[9] = 0;
        b[7] = 0;
        one = 1;
        *(u32 *)(b + 0x14) = one;
        b[0x18] = one;
    }
    t = *(u32 *)(b + 0x14);
    if (t > 15) {
        b[7] = 1;
        return;
    }
    phase = b[0x18];
    if (phase & 1)
        orr((u16 *)(b + 0xe), 0x100);
    if (t != 0) {
        u32 left = t - 1;
        *(u32 *)(b + 0x14) = left;
        if (left == 0) {
            b[0x18] = phase + 1;
            *(u32 *)(b + 0x14) = b[0x18];
        }
    }
}
