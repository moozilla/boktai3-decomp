#include "global.h"
#include "render.h"

#define gArmorPaletteResources gUnk_0824E8A8
#define gArmorPaletteBank      gUnk_0824E8F8
#define gArmorPaletteOtherBank gUnk_0824E90C

struct ArmorPaletteController {
    void (*update)(void *);
    void (*finish)(void *);
    u16 palettes[16][16]; /* 0x008 */
    u16 banks[3][2][16];  /* 0x208 */
    u8 unknown2c8[0x408 - 0x2c8];
    u16 saved[4][16]; /* 0x408 */
    u16 timer;        /* 0x488 */
    u8 bank;          /* 0x48a */
    u8 otherBank;     /* 0x48b */
};

struct Armor {
    u8 unknown0[0xee];
    u16 paletteId; /* 0x0ee */
    u8 unknownf0[0xfc - 0xf0];
    void *palette; /* 0x0fc */
    u8 unknown100[0x2e6 - 0x100];
    u16 bankId0; /* 0x2e6 */
    u8 unknown2e8[0x2ec - 0x2e8];
    void *bank0; /* 0x2ec */
    u8 unknown2f0[0x302 - 0x2f0];
    u16 bankId1; /* 0x302 */
    u8 unknown304[0x308 - 0x304];
    void *bank1; /* 0x308 */
    u8 unknown30c[0x31e - 0x30c];
    u16 bankId2; /* 0x31e */
    u8 unknown320[0x324 - 0x320];
    void *bank2; /* 0x324 */
    u8 unknown328[0x33a - 0x328];
    u16 bankId3; /* 0x33a */
    u8 unknown33c[0x340 - 0x33c];
    void *bank3; /* 0x340 */
    u8 unknown344[0x650 - 0x344];
    struct ArmorPaletteController paletteController; /* 0x650 */
    u8 unknownadc[0xb32 - 0xadc];
    u16 flags; /* 0xb32 */
    u8 unknownb34[0xb38 - 0xb34];
    u8 kind; /* 0xb38 */
};

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
