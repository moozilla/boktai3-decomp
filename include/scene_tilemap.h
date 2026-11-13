#ifndef GUARD_SCENE_TILEMAP_H
#define GUARD_SCENE_TILEMAP_H

#include "tilemap_panel.h"
#include "background_controller.h"
#include "render.h"

u32 Script_GetValueSafe(void);
void Script_SetPc(u8 *);

void SaveMenu_ClearSingleton(void);
void SaveMenu_Close(void);
void SceneEffect_ExecuteCommandList(const s16 *commands, s32 length);
u8 SceneTilemap_IsActive(void);
void TileRows_CopyPackedRows(u16 *, s32, s32, s32, s32, s32, const u16 *);
void SceneTilemap_WaitCopyDelay(void);
s32 SceneTilemap_Update(struct TilemapPanel *);
u32 SceneTilemap_Destroy(struct TilemapPanel *);
void Bg_SetControlFields(s32 id, u32 screenSize, u32 screenBase, u32 color256, u32 mosaic,
                         u32 characterBase);
void SceneTilemap_LoadLayoutFromScript(struct TilemapPanel *);
void TilemapPanel_LoadScriptTileList(struct TilemapPanel *);
s32 TilemapPanel_Initialize(struct TilemapPanel *);
struct TilemapPanel *SceneTilemap_CreateFromScript(void);
s32 SceneTilemap_GetLayoutLength(void);
s32 SceneTilemap_GetScrollDelta(void);
void SceneTilemap_SetScrollSource(struct TilemapScrollSource *);
void SceneTilemap_CopyScrollSourceState(struct TilemapScrollSource *);
s32 SceneTilemap_GetScrollLimit(void);

void LinkScorePanel_HideSprites(void);
void ScrollingDialogue_ClearScript(void);
void sub_08115AC8(s32, s32, s32, s32);
void sub_081160CC(s32, s32, s32);
s32 Mod(s32, s32);
void SpriteOam_QueueProjectedSprites(void);
void Sprite_RenderScaledList(void);
void sub_08219624(void);
void Video_SetRenderCallbacks(s32, void *, void *, void *);
void Terrain_SetColumnLayout(void *);
void Terrain_SetColumnDefinitions(const void *);
void Bg_SetTileUpload(u32, const void *, u16, u16);

extern const u8 gUnk_0860B2D0[], gUnk_08607DB0[], gUnk_08606550[];
extern const u8 gUnk_0860E3D0[], gUnk_0860A470[], gUnk_08609610[];

#endif
