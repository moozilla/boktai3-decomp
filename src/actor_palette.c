#include "actor_palette.h"

void ArmorPalette_CopyKindBanks(struct ArmorPaletteController *controller, void *data, u32 first, u32 extra)
{
    u16 *from;
    u16 *to;
    u8 *index;
    from = (u16 *)((u8 *)data + (first << 5));
    index = &controller->bank;
    to = controller->palettes[*index];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + ((first + 1) << 5));
    to = controller->palettes[*index + 1];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + ((first + 2) << 5));
    to = controller->palettes[*index + 2];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + (extra << 5));
    to = controller->palettes[controller->otherBank];
    Palette_CopyBank(to, from);
}

void ActorPalette_RestoreSelectedBanks(struct ArmorPaletteController *controller)
{
    Palette_CopyBank(controller->working[controller->bank], controller->palettes[controller->bank]);
    Palette_CopyBank(controller->working[controller->bank + 1], controller->palettes[controller->bank + 1]);
    Palette_CopyBank(controller->working[controller->bank + 2], controller->palettes[controller->bank + 2]);
    Palette_CopyBank(controller->working[controller->otherBank], controller->palettes[controller->otherBank]);
}

void ActorPalette_LoadScriptBanks(struct Armor *actor)
{
    struct ArmorPaletteController *controller = &actor->paletteController;
    struct ActorPaletteSource *source = ScriptProc_Start(0x92B3, 0xCB8F);
    u16 *bank;
    u16 *destination;
    bank = source->banks[0];
    destination = controller->palettes[actor->paletteController.bank];
    Palette_CopyBank(destination, bank);
    bank = source->banks[1];
    destination = controller->palettes[actor->paletteController.bank + 1];
    Palette_CopyBank(destination, bank);
    bank = source->banks[2];
    destination = controller->palettes[actor->paletteController.bank + 2];
    Palette_CopyBank(destination, bank);
}

extern const u32 gUnk_0824E8A8[];

extern const u8 gUnk_0824E8F8[];

extern const u8 gUnk_0824E90C[];

void Armor_BindPaletteBanks(struct Armor *actor)
{
    struct ArmorPaletteController *controller = &actor->paletteController;
    u32 index = actor->kind;
    u32 resource;
    void *source;
    u32 tag;
    u32 bank, otherBank;

    if (actor->flags & 0x200)
        index += 4;
    resource = gUnk_0824E8A8[index];
    if (resource != 0) {
        source = (u8 *)ScriptProc_Start(0x92b3, resource) + 0x14;
        tag = 0xf000 | resource;
    } else {
        source = gPaletteBankData + 0x41e0;
        tag = 0xf20f;
    }
    bank = gUnk_0824E8F8[index];
    otherBank = gUnk_0824E90C[index];
    if (actor->kind == 5) {
        controller->bank = 6;
        controller->otherBank = 0xc;
    } else {
        controller->bank = bank;
        controller->otherBank = otherBank;
    }
    ArmorPalette_CopyKindBanks(controller, source, bank, otherBank);
    actor->paletteId = tag;
    actor->palette = controller->working;
    actor->bankId0 = controller->bank + tag;
    actor->bank0 = controller->working[controller->bank];
    actor->bankId1 = controller->bank + tag + 1;
    actor->bank1 = controller->working[controller->bank + 1];
    actor->bankId2 = controller->bank + tag + 2;
    actor->bank2 = controller->working[controller->bank + 2];
    switch (actor->kind) {
        case 3:
        case 5:
        case 7:
        case 9:
        case 0xd:
        case 0xf:
            actor->bankId3 = controller->otherBank + tag;
            actor->bank3 = controller->working[controller->otherBank];
            break;
        default:
            actor->bankId3 = controller->bank + tag;
            actor->bank3 = controller->working[controller->bank];
            break;
    }
}

