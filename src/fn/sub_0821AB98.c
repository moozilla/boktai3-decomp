#include "global.h"
#include "engine.h"
/* Script_GetStringRef: Parses a string reference at the current script cursor and advances it */
#define Script_GetStringRef sub_0821AB98

s32 sub_0821AB98(void)
{
    return Script_ParseStringRef(Script_GetPc());
}
