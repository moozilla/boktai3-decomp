// COMPILER: old_agbcc
#include "global.h"
#include "link/session/session_save.h"
/* SaveData_GetCounterA: Returns the first saved halfword after the header magic; counter interpretation is tentative */

u16 SaveData_GetCounterA(void)
{
    return ((struct SessionSave *)gSessionState)->counterA;
}
