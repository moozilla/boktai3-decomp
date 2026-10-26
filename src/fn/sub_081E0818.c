#include "global.h"
#include "ram.h"
// Names are tentative, based on the static state transitions and memory writes.
struct ScriptPromptSprite {
    u8 unknown[8];
    u32 flags;
    u8 unknownC[0x20 - 0xc];
    s16 x, y;
    u8 unknown24[0x88 - 0x24];
};
struct ScriptPrompt {
    u8 unknown[0xa4];
    struct ScriptPromptSprite first, second;
    u32 state;
    u32 timer;
};
struct ScriptPromptSettings {
    const void *message, *title;
    s32 o, c, p;
};
void Menu_EraseRect(u32, u32, u32, u32);
void Menu_DrawText(u32, u32, const u8 *);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
u8 *PlayerName_GetBuffer(void);
void ScriptPrompt_ResetDisplay(struct ScriptPrompt *p)
{
    u8 text[32];
    u32 zero;
    u8 *src, *dst;
    Menu_EraseRect(4, 5, 5, 2);
    Menu_DrawText(4, 5,
                  Text_LookupString(Script_ParseStringRef(gScriptPromptSettings.message) + 8));
    Menu_EraseRect(9, 5, 16, 2);
    Menu_DrawText(9, 5, PlayerName_GetBuffer());
    if (gScriptPromptSettings.p) {
        Menu_EraseRect(3, 8, 24, 4);
        Menu_DrawText(4, 8,
                      Text_LookupString(Script_ParseStringRef(gScriptPromptSettings.message) + 7));
        zero = 0;
        CpuSet(&zero, text, 0x05000008);
        src = Text_LookupString(Script_ParseStringRef(gScriptPromptSettings.title));
        dst = text;
        dst[0] = 0x80;
        dst[1] = 0xb9;
        dst += 2;
        while (*src) {
            *dst = *src;
            src++;
            dst++;
        }
        dst[0] = 0x80;
        dst[1] = 0xba;
        Menu_DrawText(3, 10, text);
    }
    p->state = 0;
    p->first.flags &= ~1;
    p->second.flags |= 1;
    p->first.x = -32;
    p->first.y = -24;
    p->second.x = -32;
    p->second.y = -24;
    p->timer = 0;
}
