#ifndef GUARD_CHALLENGE_MENU_H
#define GUARD_CHALLENGE_MENU_H
#include "scene_input.h"
#include "game_state.h"
#include "render.h"
struct ChallengeMenu {
    struct Task task;
    struct SceneInputResource resources[3];
    struct SceneInputWidget widgets[6];
    void *backgroundResource;
    u16 *paletteSource;
    u16 palette[12];
    u16 highlightPalette[4];
    u8 highlightPhase, stateEntered, repeatTimer, enabledChoices;
    u32 previousKeys;
    u32 selectionScripts[4], cancelScript;
    s32 group, selection;
    const void *descriptionText, *optionText;
    void (*stateCallback)(struct ChallengeMenu *);
    s32 stateTimer;
};
struct ChallengeMenuPoint {
    u16 x, y;
};
struct ChallengeMenuPositions {
    struct ChallengeMenuPoint entries[5];
};
struct ChallengeMenuFrames {
    u32 frames[4];
};
struct ChallengeMenuPalette {
    u32 header[5];
    u16 colors[80];
};
extern struct ChallengeMenuPositions gUnk_0824F9C8, gUnk_0824F9DC;
extern const struct ChallengeMenuFrames gUnk_0824F9F0;
void Menu_EraseRect(int, int, int, int);
void Menu_DrawText(int, int, void *);
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
u32 ScrollingDialogue_ClearScript(void);
s32 ScrollingDialogue_SetScript(const u8 *);
void ScrollingDialogue_RequestShow(void);
void ScrollingDialogue_HideImmediately(void);
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);
void sub_082157EC(void);
void Hud_SetMode(s32);
void Bg_SetupLayers(s32, s32, void *, s32, s32, s32, s32 *);
u16 *Video_GetWrappedBackgroundMapEntry(s32, s32, s32);
void memcpy(void *, const void *, u32);
s32 ChallengeMenu_GetStoredRank(s32 a, s32 b);
void ChallengeMenu_SetStoredRank(s32 a, s32 b, u32 c);
void ChallengeMenu_DrawOptions(struct ChallengeMenu *p);
void ChallengeMenu_UpdateDescription(struct ChallengeMenu *p);
u8 ChallengeMenu_ConsumeStateEntry(struct ChallengeMenu *p);
void ChallengeMenu_SetState(struct ChallengeMenu *p, void (*v)(struct ChallengeMenu *));
void ChallengeMenu_StartFadeIn(struct ChallengeMenu *p);
void ChallengeMenu_StartFadeOut(struct ChallengeMenu *p);
void ChallengeMenu_HideWidgets(struct ChallengeMenu *p);
void ChallengeMenu_HandleInput(struct ChallengeMenu *p);
void ChallengeMenu_UpdateFadeIn(struct ChallengeMenu *p);
void ChallengeMenu_UpdateFadeOut(struct ChallengeMenu *p);
void ChallengeMenu_DispatchSelection(struct ChallengeMenu *p);
void ChallengeMenu_UpdateSelection(struct ChallengeMenu *p);
s32 ChallengeMenu_UpdateWidgets(struct ChallengeMenu *menu);
void ChallengeMenu_EnableBackground(struct ChallengeMenu *menu);
void ChallengeMenu_Update(struct ChallengeMenu *menu);
void ChallengeMenu_Destroy(struct ChallengeMenu *p);
void ChallengeMenu_InitBackgrounds(struct ChallengeMenu *p);
void ChallengeMenu_LoadPalette(struct ChallengeMenu *menu);
void ChallengeMenu_SetGroupIcon(struct ChallengeMenu *p);
void ChallengeMenu_InitializeKeyframes(struct ChallengeMenu *p);
void ChallengeMenu_InitAnimations(struct ChallengeMenu *s);
s32 ChallengeMenu_ScriptOpen(struct ChallengeMenu *s);
struct ChallengeMenu *ChallengeMenu_CreateFromScript(void);
void ChallengeMenu_ResetStoredRanks(void);
s32 ChallengeMenu_GetRankKeyframe(s32 n);
#endif
