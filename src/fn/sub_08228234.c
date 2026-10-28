// COMPILER: old_agbcc
#include "global.h"
#include "runtime.h"
/* SaveData_GetCounterA: Returns the first saved halfword after the header magic; counter interpretation is tentative */
#define SaveData_GetCounterA sub_08228234

u16 sub_08228234(void)
{
    return *(u16 *)(gSessionState + 20);
}
