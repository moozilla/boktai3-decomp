#ifndef GUARD_GAME_START_MENU_H
#define GUARD_GAME_START_MENU_H

#include "contact_swarm.h"
#include "display_sequence.h"
#include "scene_input.h"

struct GameStart {
    struct Task task;
    u32 unknown18;
    struct SceneInputWidget background;
    struct SceneInputResource backgroundResource;
    u8 unk09C[8];
    struct SceneInputWidget decoration;
    struct SceneInputResource decorationResource;
    u8 unk124[8];
    struct SceneInputWidget title;
    struct SceneInputResource titleResource;
    u8 unk1AC[8];
    struct SceneInputResource itemResource;
    struct SceneInputWidget timeDigits[5];
    struct SceneInputWidget levelDigits[2];
    struct ContactSwarmSprite cursor;
    struct ContactSwarmGraphics cursorGraphics;
    struct DisplaySequenceState cursorSequence;
    s32 selection;
    u32 unknown4D0;
    s32 fadeTimer, fadeSpeed, state, result;
    s32 saveIndex;
    s32 pulseTimer;
    void *cursorResource;
};

typedef char GameStartSizeCheck[sizeof(struct GameStart) == 0x4f0 ? 1 : -1];

struct GameStartPalette {
    u32 header[5];
    u16 colors[256];
};
struct GameStartSeqs {
    u32 seq[3];
};
struct GameStartPos {
    u32 x : 16;
    u32 y : 16;
    u32 z;
};
void GameState_BackupActiveRegions(s32);
extern const struct GameStartSeqs gUnk_0824FD50;
extern const u8 gUnk_0824FD5C[];
extern const u8 gUnk_0824FDBC[];
void *memcpy(void *, const void *, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
u8 *PlayerName_GetBuffer(void);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawText(u32, u32, const u8 *);
void Bg_SetTilemapEntry(s32, s32, s32, s32, s32, s32);
int Script_ParseStringRef(const void *);
int SceneSequence_GetSelectedMessage(void);
s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Menu_StartFadeOut(s32);
void Sound_PauseChannel10(s32);
void Script_SetEntryProcedure(s16 id);
extern void Anim_Remove(void *);
extern void Sprite_Remove(void *);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Menu_StartFadeIn(u32);
s32 GameStart_LoadAvailableSave(struct GameStart *unused);
void GameStart_RunInitHook(struct GameStart *unused);
void GameStart_DrawSaveInfo(struct GameStart *p);
void GameStart_UpdateSelectionPalette(struct GameStart *p);
void GameStart_Update(struct GameStart *p);
void GameStart_Destroy(struct GameStart *p);
s32 GameStart_Init(struct GameStart *p);
void GameStart_SetParametersFromScript(void);
void *GameStart_Create(void);
#endif
