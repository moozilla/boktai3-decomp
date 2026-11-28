#include "global.h"
#include "link/session/session_save.h"
s32 Script_GetKeywordValue(u32, s32);
s32 SaveData_SetPointsFromScript(void)
{
    s32 points = Script_GetKeywordValue('p', ((struct SessionSave *)gSessionState)->points);
    if (points < 0)
        points = 0;
    else if (points > 9999)
        points = 9999;
    ((struct SessionSave *)gSessionState)->points = points;
    return points;
}
