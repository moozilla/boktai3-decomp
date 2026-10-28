#include "global.h"

#include "engine.h"
#include "ram.h"
struct Settings {
    u8 *m;
    u8 *t;
    s32 o, c, p;
};
/* Script: -m and -t (script positions) and -o, -c, -p (values) into the
   block at (*(struct Settings *)&gScriptPromptSettings), next to ScriptMessage_SetFromScript's (tentative). */
void ScriptPrompt_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        (*(struct Settings *)&gScriptPromptSettings).m = Script_GetPc();
    if (Script_SeekToKeyword('t'))
        (*(struct Settings *)&gScriptPromptSettings).t = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        (*(struct Settings *)&gScriptPromptSettings).o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        (*(struct Settings *)&gScriptPromptSettings).c = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        (*(struct Settings *)&gScriptPromptSettings).p = Script_GetValue();
}
