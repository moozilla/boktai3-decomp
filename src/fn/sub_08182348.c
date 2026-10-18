#include "global.h"

void sub_081816B0(u8 *);
void Armor_CopyScenePalette(u32);
void Armor_UpdatePaletteRestoreDelay(u8 *);

void Armor_StartPaletteRestoreDelay(u8 *p)
{
    u8 *r4 = p + 0x650;
    u32 off;
    u32 f1;
    u32 f2;
    sub_081816B0(r4);
    off = (*(p + 0xADA) << 5) + 0x248;
    Armor_CopyScenePalette((u32)r4 + off);
    f1 = (u32)Armor_UpdatePaletteRestoreDelay;
    f2 = (u32)sub_081816B0;
    *(u32 *)r4 = f1;
    *(u32 *)(r4 + 4) = f2;
    *(u16 *)(p + 0xAD8) = 0;
}
