#include "global.h"
struct SpriteMenuBackground {
    u8 pad0[0x6E8];
    void *backgroundResource;
};
void *ScriptProc_Start(u16, u16);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void SpriteMenu_InitBackground(struct SpriteMenuBackground *menu)
{
    s32 zero;
    menu->backgroundResource = ScriptProc_Start(0xC091, 0x8544);
    zero = 0;
    Bg_SetupLayers(0, 0, menu->backgroundResource, 0, 0, 1, &zero);
}
