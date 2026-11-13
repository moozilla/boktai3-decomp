#ifndef GUARD_MODE_MENU_H
#define GUARD_MODE_MENU_H
#include "scene_input.h"
struct ScreenTextParameters {
    const void *menuText, *text;
    u32 firstScript, secondScript, thirdScript, fourthScript, cancelScript;
    s32 restoreSelection, unlockFrames, mode;
};
struct ModeMenu {
    struct Task task;
    u32 unknown18;
    s32 selection;
    u32 unknown20;
    s32 choiceCount;
    struct SceneInputResource resource;
    struct SceneInputWidget background, cursor;
    s32 state, result, fadeTimer, fadeShift;
    u16 *savedPalette;
    u16 palette[16];
    u8 pulseState[4];
    s32 frame, historyIndex, saveIndex;
    s32 unlockKeys[16], keyHistory[16], keyFrames[16];
    s32 unlocked, unlockFrames;
};
struct ScreenDelayParameters {
    const void *text;
    u32 script;
};
struct ResultWidget {
    struct SceneInputWidget sprite;
    struct SceneInputResource resource;
    u32 unknown80[2];
};
struct ResultScreen {
    struct Task task;
    struct ResultWidget widgets[2];
    s32 state;
    u32 unknown12C;
    s32 elapsed, fadeShift;
};
struct ModeMenuPalette {
    u32 header[5];
    u16 colors[256];
};

void ModeMenu_ShowDescription(struct ModeMenu *menu);
s32 ModeMenu_LoadAvailableSave(struct ModeMenu *unused);
void ModeMenu_Update(struct ModeMenu *p);
void ModeMenu_Destroy(struct ModeMenu *p);
s32 ModeMenu_Initialize(struct ModeMenu *p);
void ModeMenu_SetParametersFromScript(void);
void *ModeMenu_Create(void);
void ResultScreen_Update(struct ResultScreen *delay);
void ResultScreen_Destroy(struct ResultScreen *p);
s32 ResultScreen_Initialize(struct ResultScreen *s);
void ResultScreen_SetParametersFromScript(void);
void *ResultScreen_Create(void);
s32 ScrollingDialogue_SetScript(const u8 *);
void GameState_BackupActiveRegions(s32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawStringRef(s32, s32, const void *, s32);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestShow(void);
u32 Menu_StartFadeIn(u32);
void ScrollingDialogue_RequestHide(void);
void Menu_StartFadeOut(s32);
void Menu_SetTextRect(void *, struct ContactPosition *);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
#endif
