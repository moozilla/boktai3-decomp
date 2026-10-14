#include "global.h"
static inline void orr(u16 *a, u32 m)
{
    *a = m | *a;
}
/* Blink forever: toggle the phase every 16 frames; odd phases set display
 * flag 0x100. */
void sub_081CC5C8(u8 *a, u8 *b)
{
    u8 phase;
    u32 t;
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
        *(u32 *)(b + 0x14) = 0x10;
    }
    phase = b[0x18];
    if (phase & 1)
        orr((u16 *)(b + 0xe), 0x100);
    t = *(u32 *)(b + 0x14);
    if (t != 0) {
        t--;
        *(u32 *)(b + 0x14) = t;
        if (t == 0) {
            b[0x18] = (phase + 1) & 1;
            *(u32 *)(b + 0x14) = 0x10;
        }
    }
}
