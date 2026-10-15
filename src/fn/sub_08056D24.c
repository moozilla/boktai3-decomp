// MATCHING_CONCESSION: register-pinning
#include "script_utility.h"
#define ScriptUtility_SetFlag800 sub_08056D24
u32 ScriptUtility_SetFlag800(u32 result)
{
    register u32 *flags asm("r1") = &gUnk_030053F4;
    *flags |= 0x800;
    return result;
}
