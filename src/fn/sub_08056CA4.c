#include "script_utility.h"
#define ScriptUtility_SetControlFlagFromScript sub_08056CA4
void ScriptUtility_SetControlFlagFromScript(void)
{
    s32 value;
    if (Script_SeekToKeyword('p'))
        value = Script_GetValue();
    else
        value = 0;
    sub_08056C6C(value);
}
