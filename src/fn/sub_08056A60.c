#include "script_utility.h"
#define ScriptUtility_WriteValuesToReference sub_08056A60
s32 ScriptUtility_WriteValuesToReference(void)
{
    struct ScriptUtilityReference reference;
    s32 i;
    sub_0821AAD8(&reference);
    for (i = 0; Script_GetPc() != 0; i++)
        sub_0821B4C8(&reference, i, Script_GetValue());
    return 0;
}
