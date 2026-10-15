#include "script_utility.h"
#define ScriptUtility_IsControlFlagSet sub_08056D04
u32 ScriptUtility_IsControlFlagSet(void)
{
    if (gUnk_030053F4 & 0x200)
        return TRUE;
    return FALSE;
}
