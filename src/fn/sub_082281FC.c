#include "global.h"
struct G {
    u8 a[0xc];
    s32 w;
};
extern struct G *gSessionState;
s32 Script_GetKeywordValue(u32, s32);
s32 SaveData_SetPointsFromScript(void)
{
    s32 v = Script_GetKeywordValue(0x70, gSessionState->w);
    if (v < 0)
        v = 0;
    else if (v > 9999)
        v = 9999;
    gSessionState->w = v;
    return v;
}
