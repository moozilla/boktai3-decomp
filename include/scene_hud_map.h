#ifndef GUARD_SCENE_HUD_MAP_H
#define GUARD_SCENE_HUD_MAP_H
#include "background_controller.h"

/* Partial tilemap view of the shared 0x30-byte background record. */
extern const u8 gUnk_0824DEB8[];
extern const u16 gUnk_0824DEC0[], gUnk_0824DED0[];
u16 *SceneHud_GetMapCell(u32, u32, u32);
void SceneHud_FillTile001(u32, u32, s32);
void SceneHud_FillTile020(u32, u32, s32);
void SceneHud_DrawDecimal(s32, s32, u32, u32, s32);
void SceneHud_DrawCenteredMeter(s32);
#endif
