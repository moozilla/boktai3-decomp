#include "script_utility.h"
/* ScriptUtility_SetFlag800: Sets bit 0x800 in the shared control flags and preserves the incoming return value; pairs with ScriptUtility_WriteFlag800ToReference. */
#define ScriptUtility_SetFlag800 sub_08056D24
#define ScriptUtility_SetFlag800 sub_08056D24
u32 ScriptUtility_SetFlag800(u32 result)
{
    u32 *flags;
    do {
        do {
            do {
                flags = &gEngineFlags;
            } while (0);
        } while (0);
    } while (0);
    *flags |= 0x800;
    return result;
}
