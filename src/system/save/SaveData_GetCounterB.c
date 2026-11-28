// COMPILER: old_agbcc
#include "global.h"
#include "link/session/session_save.h"
/* SaveData_GetCounterB: Returns the second saved halfword after the header magic; counter interpretation is tentative */

u16 SaveData_GetCounterB(void)
{
    return ((struct SessionSave *)gSessionState)->counterB;
}
