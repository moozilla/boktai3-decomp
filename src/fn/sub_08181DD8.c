#include "global.h"

/* Tentative armor palette view; unknown fields retain their observed offsets.
 * Callback arguments point to the embedded controller at offset 0x650.
 * Check 4 matched all 160 bytes using the old ELF name Palette_CopyBank for
 * Palette_CopyBank; the current promoted linker name is retained here. */
struct ArmorPaletteCallbacks {
    void (*update)(void *);
    void (*complete)(void *);
};
struct ArmorPaletteState {
    u8 unknown0[0x650];
    struct ArmorPaletteCallbacks callbacks;
    u8 unknown658[0x858 - 0x658];
    u8 sourceBanks[0x200];
    u16 workingBanks[4][16];
    u16 transitionCounter;
    u8 primaryIndex;
    u8 secondaryIndex;
};

void ActorPalette_BlendSavedBanks(void *);
void ActorPalette_RestoreSelectedBanks(void *);
void Palette_CopyBank(void *, const void *);

void Armor_StartPaletteTransition(struct ArmorPaletteState *group)
{
    struct ArmorPaletteCallbacks *callbacks = &group->callbacks;
    void (*update)(void *) = ActorPalette_BlendSavedBanks;
    void (*complete)(void *) = ActorPalette_RestoreSelectedBanks;
    u32 offset;
    u8 *source;
    callbacks->update = update;
    callbacks->complete = complete;
    offset = group->primaryIndex * 32 + 0x858;
    source = (u8 *)group + offset;
    Palette_CopyBank(group->workingBanks[0], source);
    offset = group->primaryIndex * 32 + 0x878;
    source = (u8 *)group + offset;
    Palette_CopyBank(group->workingBanks[1], source);
    offset = group->primaryIndex * 32 + 0x898;
    source = (u8 *)group + offset;
    Palette_CopyBank(group->workingBanks[2], source);
    offset = group->secondaryIndex * 32 + 0x858;
    source = (u8 *)group + offset;
    Palette_CopyBank(group->workingBanks[3], source);
    group->transitionCounter = 0;
}
