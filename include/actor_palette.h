#ifndef GUARD_ACTOR_PALETTE_H
#define GUARD_ACTOR_PALETTE_H
#include "armor_palette.h"
#include "render.h"
struct ActorPaletteSource {
    u8 unknown0[0x74];
    u16 banks[3][16];
};
void Palette_CopyBank(void *, const void *);
void *ScriptProc_Start(u16, u16);
void ArmorPalette_CopyKindBanks(struct ArmorPaletteController *, void *, u32, u32);
#endif
