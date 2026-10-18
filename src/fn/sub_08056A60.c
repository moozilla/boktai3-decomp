#include "script_utility.h"
s32 ScriptUtility_WriteValuesToReference(void)
{
    struct ScriptUtilityReference reference;
    s32 i;
    Script_ReadReference(&reference);
    for (i = 0; Script_GetPc() != 0; i++)
        Script_WriteReferenceValue(&reference, i, Script_GetValue());
    return 0;
}
