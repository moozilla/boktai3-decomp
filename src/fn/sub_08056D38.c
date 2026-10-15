#include "script_utility.h"
void ScriptUtility_WriteFlag800ToReference(void)
{
    struct ScriptUtilityReference reference;
    sub_0821AAD8(&reference);
    if (gUnk_030053F4 & 0x800)
        sub_0821B4C8(&reference, 0, 1);
    else
        sub_0821B4C8(&reference, 0, 0);
}
