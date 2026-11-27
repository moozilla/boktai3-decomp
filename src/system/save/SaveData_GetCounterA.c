// COMPILER: old_agbcc
#include "global.h"
#include "engine/runtime.h"
/* SaveData_GetCounterA: Returns the first saved halfword after the header magic; counter interpretation is tentative */

u16 SaveData_GetCounterA(void)
{
    return *(u16 *)(gSessionState + 20);
}
