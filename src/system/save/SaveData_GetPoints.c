// COMPILER: old_agbcc
#include "global.h"
#include "link/session/session_save.h"
/* SaveData_GetPoints: Returns the saved value paired with the clamped points setter; points interpretation is tentative */

u32 SaveData_GetPoints(void)
{
    return ((struct SessionSave *)gSessionState)->points;
}
