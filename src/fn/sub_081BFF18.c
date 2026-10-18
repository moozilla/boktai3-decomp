#include "global.h"
void Storage_InitEntries(u8 *);
void Storage_InitBackground(u8 *);
void Storage_LoadDisplayResources(u8 *);
void sub_081BF67C(u8 *);
void Storage_InitTextOptions(u8 *);
void SceneInventory_InitWidgetPalette(u8 *, u8 *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void sub_081BFC14(u8 *, s32);
void Sound_PlaySong(s32);
void Storage_SetUpdateCallback(u8 *, void (*)(void));
void Storage_UpdateOpeningDelay(void);
/* Screen setup: five sub-inits, SceneInventory_InitWidgetPalette on +0x17C8, ScreenTransition_SendCommand(0, 5,
   4, 4, 4, 0xFFFF, 0), song 0x118 and update Storage_UpdateOpeningDelay. */
u32 Storage_Init(u8 *p)
{
    Storage_InitEntries(p);
    Storage_InitBackground(p);
    Storage_LoadDisplayResources(p);
    sub_081BF67C(p);
    Storage_InitTextOptions(p);
    SceneInventory_InitWidgetPalette(p, p + 0x17C8);
    ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, 0);
    sub_081BFC14(p, 1);
    Sound_PlaySong(0x118);
    Storage_SetUpdateCallback(p, Storage_UpdateOpeningDelay);
    return 0;
}
