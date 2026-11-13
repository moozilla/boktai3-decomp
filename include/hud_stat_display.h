#ifndef GUARD_HUD_STAT_DISPLAY_H
#define GUARD_HUD_STAT_DISPLAY_H
#include "scene_hud_display.h"
#include "player.h"
void Hud_SetUpdateCallback(struct SceneHudState *p, u32 w);
void Hud_SetInteractionCallback(struct SceneHudState *p, u32 w, u32 c);
void Hud_SetupBackgroundLayer(s32 unused, void *map, s32 offset, s32 format, const void *palette);
u16 *Hud_GetTilemapCell(s32 i, u32 x, u32 y);
s32 Hud_GetStyleTileIndex(s32 a, s32 b);
void Hud_InitSelectionBounds(u32 a, u32 b);
s32 Hud_GetMinimum(s32 a, s32 b);
void Hud_DrawStatRatio(void *unused, s32 style, s32 divisor, s32 count, s32 numerator,
                       s32 denominator, s32 x, s32 y);
void Hud_DrawHealthRatio(struct SceneHudState *p, u32 a, u32 b, u32 c);
void Hud_DrawEnergyRatio(struct SceneHudState *p, u32 a, u32 b, u32 c);
void Hud_DrawPlayerValue(struct SceneHudState *p, s32 style, s32 x, s32 y);
#endif
