#ifndef GUARD_SCRIPT_PROMPT_H
#define GUARD_SCRIPT_PROMPT_H

#include "scene_input.h"
struct ScriptPromptWidget {
    struct SceneInputWidget sprite;
    struct SceneInputResource resource;
    u32 unknown80[2];
};
struct ScriptPromptPalette {
    u32 header[5];
    u16 colors[256];
};

struct ScriptPromptSettings {
    const void *message, *title;
    u32 acceptScript, cancelScript;
    s32 showDescription;
};
struct ScriptPromptScreen {
    struct Task task;
    u32 unknown18;
    struct ScriptPromptWidget widgets[3];
    s32 selection, timer, fadeTimer, fadeSpeed, state, result;
    u32 unknown1CC;
    u16 *savedPalette;
    u16 colors[16];
    u8 pulseState[4];
};

void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawText(s32, s32, const u8 *);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
u8 *PlayerName_GetBuffer(void);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawText(s32, s32, const u8 *);
void Sound_PauseChannel10(u32);
void Menu_StartFadeOut(s32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Menu_StartFadeIn(u32);
void ScriptPrompt_ResetDisplay(struct ScriptPromptScreen *p);
void ScriptPrompt_DrawFooterText(struct ScriptPromptScreen *unused);
void ScriptPrompt_UpdateSelection(struct ScriptPromptScreen *p);
void ScriptPrompt_Update(struct ScriptPromptScreen *s);
void ScriptPrompt_Destroy(struct ScriptPromptScreen *p);
s32 ScriptPrompt_Initialize(struct ScriptPromptScreen *s);
void ScriptPrompt_SetFromScript(void);
void *ScriptPrompt_Create(void);
#endif
