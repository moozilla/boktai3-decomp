#include "armor_palette.h"

struct ActorPaletteSource {
    u8 unknown0[0x74];
    u16 banks[3][16];
};
void *ScriptProc_Start(u16, u16);
void Palette_CopyBank(u16 *, u16 *);
void ActorPalette_LoadScriptBanks(struct Armor *p)
{
    struct ArmorPaletteController *rows = &p->paletteController;
    struct ActorPaletteSource *source = ScriptProc_Start(0x92B3, 0xCB8F);
    u16 *bank;
    u16 *destination;
    bank = source->banks[0];
    destination = rows->palettes[p->paletteController.bank];
    Palette_CopyBank(destination, bank);
    bank = source->banks[1];
    destination = rows->palettes[p->paletteController.bank + 1];
    Palette_CopyBank(destination, bank);
    bank = source->banks[2];
    destination = rows->palettes[p->paletteController.bank + 2];
    Palette_CopyBank(destination, bank);
}
