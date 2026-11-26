// COMPILER: old_agbcc
#include "global.h"
#include "engine/runtime.h"
/* SaveData_GetCounterB: Returns the second saved halfword after the header magic; counter interpretation is tentative */

u16 SaveData_GetCounterB(void)
{
    return *(u16 *)(gSessionState + 0x16);
}
