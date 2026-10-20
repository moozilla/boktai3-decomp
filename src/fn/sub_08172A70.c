#include "global.h"
#define PauseInventory_EnterEquipmentSelection sub_08172A70
void SceneHud_ConfigureActionGrid(u8 *, u32, u32);
void Hud_SetGridPosition(u8 *, u32, u32);
void SceneHud_SetActionCursorPosition(u8 *, u32);
void PauseMenu_RefreshFruitAndUnlockIcons(u8 *);
void PauseMenu_AddSpriteFlags(u8 *, u32, u32);
void PauseMenu_ClearSpriteFlags(u8 *, u32, u32);
void Anim_LoadKeyframe(u8 *, u8 *, u32, u32);
void PauseInventory_EnterEquipmentSelection(u8 *s)
{
    u8 *grid = s + 0x4384;
    u8 *keys;
    SceneHud_ConfigureActionGrid(grid, 0x61, 0);
    Hud_SetGridPosition(grid, 8, 0);
    SceneHud_SetActionCursorPosition(s + 0x15C0, s[0x4387]);
    PauseMenu_RefreshFruitAndUnlockIcons(s);
    PauseMenu_AddSpriteFlags(s, 5, 1);
    Anim_LoadKeyframe(s + 0x1600, keys = s + 0x64, 0x4D, 0);
    PauseMenu_AddSpriteFlags(s, 6, 1);
    Anim_LoadKeyframe(s + 0x1660, keys, 0x4E, 0);
    PauseMenu_ClearSpriteFlags(s, 4, 1);
}
