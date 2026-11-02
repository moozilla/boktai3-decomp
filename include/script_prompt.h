#ifndef GUARD_SCRIPT_PROMPT_H
#define GUARD_SCRIPT_PROMPT_H

#include "text_entry_widget.h"

struct ScriptPromptSettings {
    const void *message, *title;
    u32 acceptScript, cancelScript;
    s32 showDescription;
};
struct ScriptPromptScreen {
    u8 task[0x1C];
    struct TextEntryWidget widgets[3];
    s32 selection, timer, fadeTimer, fadeSpeed, state, result;
    u8 unknown1CC[4];
    u16 *savedPalette;
    u16 colors[16];
    u8 pulseState[4];
};

void ScriptPrompt_DrawFooterText(struct ScriptPromptScreen *);
s32 ScriptPrompt_Initialize(struct ScriptPromptScreen *);
void ScriptPrompt_ResetDisplay(struct ScriptPromptScreen *);

#endif
