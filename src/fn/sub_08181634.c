#include "global.h"
// MATCHING_CONCESSION: register-pinning
void Palette_CopyBank(void *, void *);
void ArmorPalette_CopyKindBanks(u8 *s0, u8 *dst0, u32 i0, u32 j0)
{
    register u8 *s asm("r6") = s0;
    register u8 *dst asm("r8") = dst0;
    register u32 i asm("r4") = i0;
    register u32 j asm("r9") = j0;
    u8 *d;
    u8 *bank;
    d = dst + i * 32;
    bank = s + 0x48A;
    Palette_CopyBank(s + (*bank * 32 + 8), d);
    d = dst + (i + 1) * 32;
    Palette_CopyBank(s + (*bank * 32 + 0x28), d);
    d = dst + (i + 2) * 32;
    Palette_CopyBank(s + (*bank * 32 + 0x48), d);
    j <<= 5;
    d = dst + j;
    Palette_CopyBank(s + (s[0x48B] * 32 + 8), d);
}
