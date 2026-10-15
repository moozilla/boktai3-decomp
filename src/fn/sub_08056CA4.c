#include "script_utility.h"
void ScriptUtility_SetControlFlagFromScript(void)
{
    s32 value;
    if (Script_SeekToKeyword('p'))
        value = Script_GetValue();
    else
        value = 0;
    ScriptUtility_SetControlFlag(value);
}
