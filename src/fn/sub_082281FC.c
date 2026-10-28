#include "global.h"
#include "runtime.h"
struct G {
    u8 a[0xc];
    s32 w;
};
s32 Script_GetKeywordValue(u32, s32);
s32 SaveData_SetPointsFromScript(void)
{
    s32 v = Script_GetKeywordValue(0x70, ((struct G *)gSessionState)->w);
    if (v < 0)
        v = 0;
    else if (v > 9999)
        v = 9999;
    ((struct G *)gSessionState)->w = v;
    return v;
}
