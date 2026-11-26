#include "global.h"

s32 Script_CreateDeferredTaskFromScript(u8 *, s32, s32);

s32 Script_ScheduleRepeatingFromScript(u8 *a, s32 b)
{
    return Script_CreateDeferredTaskFromScript(a, b, 1);
}
