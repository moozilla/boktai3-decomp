#include "global.h"

#include "engine.h"
#include "menu_parameters.h"

/* Script: text from string -c (Text_LookupString) into gScriptMessageParameters, with
   -d (script position) and -e (value) (tentative). */
void ScriptMessage_SetFromScript(void)
{
    u8 *pc = 0;
    if (Script_SeekToKeyword('c'))
        pc = Script_GetPc();
    gScriptMessageParameters.text = Text_LookupString(Script_ParseStringRef(pc));
    if (Script_SeekToKeyword('d'))
        gScriptMessageParameters.pc = Script_GetPc();
    if (Script_SeekToKeyword('e'))
        gScriptMessageParameters.value = Script_GetValue();
}
