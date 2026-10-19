#include "global.h"

#ifdef NONMATCHING
// fuzzy: 81.0%
// foundry draft (worker best 81.0%); what differs is mostly register allocation and instruction order
#define ActorPalette_CopyBanks ArmorPalette_CopyKindBanks
struct PaletteActor {
    u8 pad0[0x48A];
    u8 bank, secondaryBank;
};
void Palette_CopyBank(void *, u32);
void ArmorPalette_CopyKindBanks(struct PaletteActor *p, u8 *source, s32 bank, s32 secondary)
{
    u8 *src;
    src = source + (bank << 5);
    Palette_CopyBank((u8 *)p + ((p->bank << 5) + 8), (u32)src);
    src = source + ((bank + 1) << 5);
    Palette_CopyBank((u8 *)p + ((p->bank << 5) + 0x28), (u32)src);
    bank += 2;
    src = source + (bank << 5);
    Palette_CopyBank((u8 *)p + ((p->bank << 5) + 0x48), (u32)src);
    src = source + (secondary << 5);
    Palette_CopyBank((u8 *)p + ((p->secondaryBank << 5) + 8), (u32)src);
}
#else
INCLUDE_ASM("asm/nonmatching", ArmorPalette_CopyKindBanks);
#endif
