// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/runtime.h"
/* SaveData_GetCounterB: Returns the second saved halfword after the header magic; counter interpretation is tentative */
#define SaveData_GetCounterB sub_08228240

u16 sub_08228240(void)
{
    return *(u16 *)(gSessionState + 0x16);
}
