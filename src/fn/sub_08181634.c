#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.4% (only the order of the initial register moves differs)
// MATCHING_CONCESSION: register-pinning
void Palette_CopyBank(void *, void *);
void ArmorPalette_CopyKindBanks(u8 *s0, u8 *dst, u32 i, u32 j)
{
    register u8 *s asm("r6") = s0;
    u8 *d;
    u8 *bank;
    d = dst + i * 32;
    bank = s + 0x48A;
    Palette_CopyBank(s + (*bank * 32 + 8), d);
    d = dst + (i + 1) * 32;
    Palette_CopyBank(s + (*bank * 32 + 0x28), d);
    d = dst + (i + 2) * 32;
    Palette_CopyBank(s + (*bank * 32 + 0x48), d);
    d = dst + j * 32;
    Palette_CopyBank(s + (s[0x48B] * 32 + 8), d);
}
#else
INCLUDE_ASM("asm/nonmatching", ArmorPalette_CopyKindBanks);
#endif
