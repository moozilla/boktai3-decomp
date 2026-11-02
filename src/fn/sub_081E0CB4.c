#include "script_prompt.h"

#include "engine.h"
#include "ram.h"
void ScriptPrompt_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gScriptPromptSettings.message = Script_GetPc();
    if (Script_SeekToKeyword('t'))
        gScriptPromptSettings.title = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        gScriptPromptSettings.acceptScript = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gScriptPromptSettings.cancelScript = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        gScriptPromptSettings.showDescription = Script_GetValue();
}
