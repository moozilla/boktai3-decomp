#include "script_prompt.h"
#include "ram.h"
void Menu_EraseRect(s32, s32, s32, s32);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
void Menu_DrawText(s32, s32, const u8 *);
void ScriptPrompt_DrawFooterText(struct ScriptPromptScreen *unused)
{
    Menu_EraseRect(4, 0xd, 0x16, 2);
    Menu_DrawText(4, 0xd,
                  Text_LookupString(Script_ParseStringRef(gScriptPromptSettings.message) + 0xa));
}
