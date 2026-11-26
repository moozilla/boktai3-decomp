// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/runtime.h"
/* SaveData_GetCounterA: Returns the first saved halfword after the header magic; counter interpretation is tentative */
#define SaveData_GetCounterA sub_08228234

u16 sub_08228234(void)
{
    return *(u16 *)(gSessionState + 20);
}
