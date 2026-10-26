#include "global.h"
#include "render.h"
#define gSpriteMenuPaletteBuffer gUnk_03005160
struct SpriteMenuPalette {
    u8 pad0[0x6EC];
    void *paletteResource;
    u8 pad6F0[0x1E];
    u8 paletteState;
    u8 pad70F;
    u32 savedPalette[8];
};
extern u32 gUnk_03005160[];
void *ScriptProc_Start(u16, u16);
void SpriteMenu_LoadPalette(struct SpriteMenuPalette *menu)
{
    menu->paletteResource = (u8 *)ScriptProc_Start(0x92B3, 0x2E9) + 0x1B4;
    CpuSet(menu->paletteResource, gUnk_03005160, 0x04000018);
    CpuSet(gPaletteBankData + 0x7540, menu->savedPalette, 0x04000008);
    menu->paletteState = 0;
}
