#ifndef GUARD_PAUSE_MENU_H
#define GUARD_PAUSE_MENU_H
#include "hud_stat_display.h"
u32 PauseMenu_ReadDirectionRepeat(struct SceneHudState *p);
void PauseMenu_SetConfirmationIndicator(struct SceneHudState *p, s32 b);
s32 PauseMenu_UpdateBinaryChoice(struct SceneHudState *hud);
void PauseMenu_HideAllSprites(struct SceneHudState *p);
void PauseMenu_LoadSpriteKeyframe(struct SceneHudState *p, s32 i, u32 c);
void PauseMenu_SetSpritePosition(struct SceneHudState *p, s32 i, u16 a, u16 b);
void PauseMenu_AddSpriteFlags(struct SceneHudState *p, s32 i, u32 m);
void PauseMenu_ClearSpriteFlags(struct SceneHudState *p, s32 i, u32 m);
void PauseMenu_ClearTileRect(s32 x, s32 y, s32 width, s32 height);
void PauseMenu_SetSpritePalette(struct SceneInputWidget *o, u32 v);
void PauseMenu_SmoothPoint(s16 *a, s16 *b, s32 n);
u32 PlayerPanel_GetStatusValue(struct SceneHudState *panel);
void PauseMenu_StartPaletteTransition(struct SceneHudState *p, u16 a, u16 b, s32 d);
s32 PauseMenu_TickPaletteBlend(struct SceneHudState *p);
void PauseMenu_BeginSpriteMotion(struct SceneHudState *hud, u32 index, u32 keyframe, u32 palette,
                                 struct SceneVec *from, struct SceneVec *to, s32 bend,
                                 u32 duration);
u32 PauseMenu_UpdateSpriteMotion(struct SceneHudState *p, u32 i);
void PauseMenu_OpenPrompt(s32 a, s32 b, s32 c);
void PauseMenu_SetPromptIconPositions(struct SceneHudState *p, u32 a, u32 b);
s32 PauseMenu_GetPromptIconKeyframe(s32 a);
void PauseMenu_SetPairedIcons(struct SceneHudState *hud, s32 left, s32 right);
void PauseMenu_HidePromptIcons(struct SceneHudState *p);
void PauseMenu_CloseTextWindows(struct SceneHudState *p);
void PauseMenu_ShowPromptSprite(struct SceneHudState *p, s16 x, s16 y);
void PauseMenu_LoadCategoryIconFrame(struct SceneHudState *p, struct SceneInputWidget *x, s32 c);
void PauseMenu_LoadEquipmentIconFrame(struct SceneHudState *p, struct SceneInputWidget *x, s32 c);
void PauseMenu_LoadAttributeIconFrame(struct SceneHudState *p, struct SceneInputWidget *x, s32 c);
void PauseMenu_LoadStatusIconFrame(struct SceneHudState *p, struct SceneInputWidget *x, s32 c);
void PauseMenu_LoadEmptySlotFrame(struct SceneHudState *p, struct SceneInputWidget *x);
void PauseMenu_LoadFixedIconKeyframe(struct SceneHudState *p, struct SceneInputWidget *x);
void PauseMenu_LoadIndexedIconKeyframe(struct SceneHudState *p, struct SceneInputWidget *x, s32 c);
#endif
