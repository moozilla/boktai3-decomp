#include "armor_palette.h"
#include "render.h"

#define gArmorPaletteResources gUnk_0824E8A8
#define gArmorPaletteBank      gUnk_0824E8F8
#define gArmorPaletteOtherBank gUnk_0824E90C

extern const u32 gUnk_0824E8A8[];
extern const u8 gUnk_0824E8F8[];
extern const u8 gUnk_0824E90C[];
void *ScriptProc_Start(u16 key, u16 context);
void ArmorPalette_CopyKindBanks(struct ArmorPaletteController *c, void *src, u32 bank,
                                u32 otherBank);

void Armor_BindPaletteBanks(struct Armor *p)
{
    struct ArmorPaletteController *c = &p->paletteController;
    u32 index = p->kind;
    u32 resource;
    void *src;
    u32 tag;
    u32 bank, otherBank;

    if (p->flags & 0x200)
        index += 4;
    resource = gUnk_0824E8A8[index];
    if (resource != 0) {
        src = (u8 *)ScriptProc_Start(0x92b3, resource) + 0x14;
        tag = 0xf000 | resource;
    } else {
        src = gPaletteBankData + 0x41e0;
        tag = 0xf20f;
    }
    bank = gUnk_0824E8F8[index];
    otherBank = gUnk_0824E90C[index];
    if (p->kind == 5) {
        c->bank = 6;
        c->otherBank = 0xc;
    } else {
        c->bank = bank;
        c->otherBank = otherBank;
    }
    ArmorPalette_CopyKindBanks(c, src, bank, otherBank);
    p->paletteId = tag;
    p->palette = (u8 *)c + 0x208;
    p->bankId0 = c->bank + tag;
    p->bank0 = (u8 *)c + (c->bank * 32 + 0x208);
    p->bankId1 = c->bank + tag + 1;
    p->bank1 = (u8 *)c + (c->bank * 32 + 0x228);
    p->bankId2 = c->bank + tag + 2;
    p->bank2 = (u8 *)c + (c->bank * 32 + 0x248);
    switch (p->kind) {
        case 3:
        case 5:
        case 7:
        case 9:
        case 0xd:
        case 0xf:
            p->bankId3 = c->otherBank + tag;
            p->bank3 = (u8 *)c + (c->otherBank * 32 + 0x208);
            break;
        default:
            p->bankId3 = c->bank + tag;
            p->bank3 = (u8 *)c + (c->bank * 32 + 0x208);
            break;
    }
}
