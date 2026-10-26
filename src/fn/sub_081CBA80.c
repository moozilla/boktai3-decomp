#include "global.h"
#include "engine.h"
s32 Mod(s32, s32);
void Palette_InterpolateBanks(void *dst, void *from, void *to, s32 t, s32 shift);
/* Copy the 0x680 palette bank in, then pulse between it and the 0x2DE0 bank. */
void SceneCharacter_UpdateAlternatePalettePulse(u8 *a, u8 *b)
{
    s32 t;
    if (b[9]) {
        u32 zero = 0;
        b[9] = zero;
        b[7] = zero;
        CpuSet(gPaletteBankData + 0x680, b + 0x1cc, 0x04000008);
        b[0x18] = zero;
        *(u32 *)(b + 0x10) = zero;
        b[7] = 1;
    }
    t = Mod(*(u32 *)(b + 0x10), 128);
    if (t <= 64) {
        u8 *bank = gPaletteBankData;
        u8 *from = bank + 0x680, *to = bank + 0x2de0;
        Palette_InterpolateBanks(b + 0x1cc, from, to, t, 6);
    } else {
        u8 *bank = gPaletteBankData;
        u8 *from = bank + 0x2de0, *to = bank + 0x680;
        Palette_InterpolateBanks(b + 0x1cc, from, to, t - 64, 6);
    }
}
