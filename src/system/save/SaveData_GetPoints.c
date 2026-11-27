// COMPILER: old_agbcc
#include "global.h"
#include "engine/runtime.h"
/* SaveData_GetPoints: Returns the saved value paired with the clamped points setter; points interpretation is tentative */

u32 SaveData_GetPoints(void)
{
    return *(u32 *)(gSessionState + 12);
}
