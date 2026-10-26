#include "global.h"

#include "engine.h"
struct Settings {
    u8 *m;
    u8 *t;
    s32 o, c, p;
};
extern struct Settings gScriptPromptSettings;
/* Script: -m and -t (script positions) and -o, -c, -p (values) into the
   block at gScriptPromptSettings, next to ScriptMessage_SetFromScript's (tentative). */
void ScriptPrompt_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gScriptPromptSettings.m = Script_GetPc();
    if (Script_SeekToKeyword('t'))
        gScriptPromptSettings.t = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        gScriptPromptSettings.o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gScriptPromptSettings.c = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        gScriptPromptSettings.p = Script_GetValue();
}
