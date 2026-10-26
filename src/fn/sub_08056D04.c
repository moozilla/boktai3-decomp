#include "script_utility.h"
u32 ScriptUtility_IsControlFlagSet(void)
{
    if (gEngineFlags & 0x200)
        return TRUE;
    return FALSE;
}
