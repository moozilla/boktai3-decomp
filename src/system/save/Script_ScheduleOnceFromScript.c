#include "global.h"

s32 Script_CreateDeferredTaskFromScript(u8 *, s32, s32);

s32 Script_ScheduleOnceFromScript(u8 *a, s32 b)
{
    return Script_CreateDeferredTaskFromScript(a, b, 0);
}
