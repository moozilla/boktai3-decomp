#include "script_utility.h"
void ScriptUtility_WriteFlag800ToReference(void)
{
    struct ScriptUtilityReference reference;
    Script_ReadReference(&reference);
    if (gEngineFlags & 0x800)
        Script_WriteReferenceValue(&reference, 0, 1);
    else
        Script_WriteReferenceValue(&reference, 0, 0);
}
