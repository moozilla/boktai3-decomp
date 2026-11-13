#ifndef GUARD_PAUSE_MENU_H
#define GUARD_PAUSE_MENU_H
#include "hud_stat_display.h"

u32 PauseMenu_ReadDirectionRepeat(struct SceneHudState *hud);
void PauseMenu_SetConfirmationIndicator(struct SceneHudState *hud, s32 choice);
s32 PauseMenu_UpdateBinaryChoice(struct SceneHudState *hud);
void PauseMenu_HideAllSprites(struct SceneHudState *hud);
void PauseMenu_LoadSpriteKeyframe(struct SceneHudState *hud, s32 index, u32 keyframe);
void PauseMenu_SetSpritePosition(struct SceneHudState *hud, s32 index, u16 x, u16 y);
void PauseMenu_AddSpriteFlags(struct SceneHudState *hud, s32 index, u32 mask);
void PauseMenu_ClearSpriteFlags(struct SceneHudState *hud, s32 index, u32 mask);
void PauseMenu_ClearTileRect(s32 x, s32 y, s32 width, s32 height);
void PauseMenu_SetSpritePalette(struct SceneInputWidget *widget, u32 bank);
void PauseMenu_SmoothPoint(s16 *position, s16 *target, s32 weight);
u32 PauseMenu_GetPlayerStatusValue(struct SceneHudState *hud);
void PauseMenu_StartPaletteTransition(struct SceneHudState *hud, u16 sourceBank, u16 targetBank,
                                      s32 duration);
s32 PauseMenu_TickPaletteBlend(struct SceneHudState *hud);
void PauseMenu_BeginSpriteMotion(struct SceneHudState *hud, u32 index, u32 keyframe, u32 palette,
                                 struct SceneVec *from, struct SceneVec *to, s32 bend,
                                 u32 duration);
u32 PauseMenu_UpdateSpriteMotion(struct SceneHudState *hud, u32 index);
void PauseMenu_OpenPrompt(const void *messageTable, s32 message, s32 playSound);
void PauseMenu_SetPromptIconPositions(struct SceneHudState *hud, u32 x, u32 y);
s32 PauseMenu_GetPromptIconKeyframe(s32 icon);
void PauseMenu_SetPairedIcons(struct SceneHudState *hud, s32 left, s32 right);
void PauseMenu_HidePromptIcons(struct SceneHudState *hud);
void PauseMenu_CloseTextWindows(struct SceneHudState *hud);
void PauseMenu_ShowPromptSprite(struct SceneHudState *hud, s16 x, s16 y);
void PauseMenu_LoadCategoryIconFrame(struct SceneHudState *hud, struct SceneInputWidget *widget,
                                     s32 icon);
void PauseMenu_LoadEquipmentIconFrame(struct SceneHudState *hud, struct SceneInputWidget *widget,
                                      s32 icon);
void PauseMenu_LoadAttributeIconFrame(struct SceneHudState *hud, struct SceneInputWidget *widget,
                                      s32 icon);
void PauseMenu_LoadStatusIconFrame(struct SceneHudState *hud, struct SceneInputWidget *widget,
                                   s32 icon);
void PauseMenu_LoadEmptySlotFrame(struct SceneHudState *hud, struct SceneInputWidget *widget);
void PauseMenu_LoadFixedIconKeyframe(struct SceneHudState *hud, struct SceneInputWidget *widget);
void PauseMenu_LoadIndexedIconKeyframe(struct SceneHudState *hud, struct SceneInputWidget *widget,
                                       s32 icon);
#endif
