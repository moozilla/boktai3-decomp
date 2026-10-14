#include "global.h"
#include "engine.h"
s32 Mod(s32, s32);
void sub_08221110(void *dst, void *from, void *to, s32 t, s32 shift);
/* Pulse the palette between the 0x680 and 0x2E80 banks: 64 frames each way. */
void sub_081CBA04(u8 *a, u8 *b)
{
    s32 t;
    if (b[9]) {
        u32 zero = 0;
        b[9] = zero;
        b[0x18] = zero;
        *(u32 *)(b + 0x10) = zero;
        b[7] = 1;
    }
    t = Mod(*(u32 *)(b + 0x10), 128);
    if (t <= 64) {
        u8 *bank = gUnk_030042E4;
        u8 *from = bank + 0x680, *to = bank + 0x2e80;
        sub_08221110(b + 0x1cc, from, to, t, 6);
    } else {
        u8 *bank = gUnk_030042E4;
        u8 *from = bank + 0x2e80, *to = bank + 0x680;
        sub_08221110(b + 0x1cc, from, to, t - 64, 6);
    }
}
