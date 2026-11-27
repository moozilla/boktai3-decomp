#include "global.h"

#include "engine/engine.h"
void SpriteMenu_InitBackground(u8 *);
void SpriteMenu_LoadPalette(u8 *);
void SolarBank_InitWidgets(u8 *);
void SpriteMenu_InitText(u8 *);
void SolarBank_BeginOpening(u8 *);
/* Sets up the Solar Bank screen (four sub-inits), keeps -e at +0x6FC and
   runs SolarBank_BeginOpening; called by SolarBank_Create (tentative). */
u32 SolarBank_Init(u8 *p)
{
    u32 e;
    SpriteMenu_InitBackground(p);
    SpriteMenu_LoadPalette(p);
    SolarBank_InitWidgets(p);
    SpriteMenu_InitText(p);
    if ((e = Script_SeekToKeyword('e')) != 0)
        *(u32 *)(p + 0x6FC) = Script_GetValue();
    else
        *(u32 *)(p + 0x6FC) = e;
    SolarBank_BeginOpening(p);
    return 0;
}
