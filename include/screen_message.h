#ifndef GUARD_SCREEN_MESSAGE_H
#define GUARD_SCREEN_MESSAGE_H
#include "scene_input.h"
struct ScreenParams {
    u8 *script;
    s32 message, showChoices;
    u32 confirmScript, alternateScript, cancelScript;
};
struct ScreenMessageWidget {
    struct SceneInputWidget sprite;
    struct SceneInputResource graphics;
    u32 unknown80[2];
};
struct ScreenMessage {
    struct Task task;
    u32 unknown18;
    struct ScreenMessageWidget choices[2];
    s32 state, result, fadeTimer, fadeSpeed, selection;
    u32 textWindow;
    u16 *originalPalette;
    u16 palette[16];
    u8 pulseState, unknown169[3];
};
struct ScreenMessagePalette {
    u32 header[5];
    u16 colors[256];
};
void Palette_SetBackdropColor(u32);
s32 TextWindow_Destroy(s32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Menu_StartFadeIn(u32);
void ScreenMessage_UpdateChoiceHighlight(struct ScreenMessage *p);
void ScreenMessage_Update(struct ScreenMessage *s);
void ScreenMessage_Destroy(struct ScreenMessage *p);
s32 ScreenMessage_Initialize(struct ScreenMessage *s);
void ScreenMessage_SetParametersFromScript(void);
struct ScreenMessage *ScreenMessage_Create(void);
#endif
