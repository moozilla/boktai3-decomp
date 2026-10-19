#include "global.h"
void Armor_BindPaletteBanks(void);
void sub_0818171C(void *);
void ActorPalette_RestoreSelectedBanks(void *);
void Armor_InitPaletteController(u8 *s)
{
    u32 *t;
    Armor_BindPaletteBanks();
    switch (*(u8 *)(s + 0xb38)) {
        case 3:
        case 5:
        case 7:
        case 9:
        case 0xd:
        case 0xf:
            sub_0818171C(s);
            break;
        default:
            break;
    }
    t = (u32 *)(s + 0x650);
    ActorPalette_RestoreSelectedBanks(t);
    t[0] = 0;
    t[1] = 0;
}
