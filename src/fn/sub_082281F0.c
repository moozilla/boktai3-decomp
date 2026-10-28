// COMPILER: old_agbcc
#include "global.h"
#include "runtime.h"
/* SaveData_GetPoints: Returns the saved value paired with the clamped points setter; points interpretation is tentative */
#define SaveData_GetPoints sub_082281F0

u32 sub_082281F0(void)
{
    return *(u32 *)(gSessionState + 12);
}
