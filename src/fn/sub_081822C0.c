#include "global.h"

struct ArmorPaletteController {
    void (*update)(struct ArmorPaletteController *);
    void (*completion)(struct ArmorPaletteController *);
    u8 unk008[0x488 - 8];
    u16 restoreTimer;
    u8 primaryBank;
    u8 secondaryBank;
};

struct ArmorPaletteOwner {
    u8 unk000[0x650];
    struct ArmorPaletteController palette;
};

void ActorPalette_RestoreSelectedBanks(struct ArmorPaletteController *);
void Armor_UpdatePaletteRestoreDelay(struct ArmorPaletteController *);
void Armor_CopyScenePalette(u32);

/* Tentative: apply the scene palette to all four banks, then delay restoration. */
void Armor_StartFullPaletteRestoreDelay(struct ArmorPaletteOwner *owner)
{
    struct ArmorPaletteController *palette = &owner->palette;
    u32 offset;
    void (*update)(struct ArmorPaletteController *);
    void (*completion)(struct ArmorPaletteController *);

    ActorPalette_RestoreSelectedBanks(palette);
    offset = (palette->primaryBank << 5) + 0x208;
    Armor_CopyScenePalette((u32)palette + offset);
    offset = (palette->primaryBank << 5) + 0x228;
    Armor_CopyScenePalette((u32)palette + offset);
    offset = (palette->primaryBank << 5) + 0x248;
    Armor_CopyScenePalette((u32)palette + offset);
    offset = (palette->secondaryBank << 5) + 0x208;
    Armor_CopyScenePalette((u32)palette + offset);
    update = Armor_UpdatePaletteRestoreDelay;
    completion = ActorPalette_RestoreSelectedBanks;
    palette->update = update;
    palette->completion = completion;
    palette->restoreTimer = 0;
}
