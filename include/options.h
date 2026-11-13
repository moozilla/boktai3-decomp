#ifndef GUARD_OPTIONS_H
#define GUARD_OPTIONS_H
#include "scene_input.h"
#include "menu_parameters.h"
#include "script_utility.h"
struct OptionsTextRect {
    u16 x, y, width, height;
};
struct OptionsWidget {
    struct SceneInputWidget sprite;
    struct SceneInputResource graphics;
    u32 unknown80[2];
};
struct OptionsScreen {
    struct Task task;
    u32 unknown18;
    s32 selection, choice;
    struct OptionsWidget background, menu, cursor, leftArrow, rightArrow;
    s32 state, exitChoice, timer;
    u32 fade;
    u16 *originalPalette;
    u16 highlightPalette[16];
    u8 highlightPhase, unknown301[3];
};
struct OptionsPalette {
    u32 header[5];
    u16 colors[256];
};
void Menu_StartFadeOut(s32);
void Menu_StartFadeIn(s32);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawStringRef(s32, s32, const u8 *, s32);
void Menu_SetTextRect(void *, void *);
void Bg_SetupLayers(s32, s32, void *, s32, s32, s32, void *);
void Options_UpdateScreen(struct OptionsScreen *p);
void Options_Destroy(struct OptionsScreen *p);
s32 Options_InitScreen(struct OptionsScreen *screen);
void Options_SetParametersFromScript(void);
u32 Options_GetSecondaryChoice(void);
struct OptionsScreen *Options_Create(void);
#endif
