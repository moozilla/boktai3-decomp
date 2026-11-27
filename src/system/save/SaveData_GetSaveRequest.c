#include "global.h"
#include "engine/runtime.h"
/* SaveData_GetSaveRequest: Returns header byte 9 paired with the tentative save request setter */
u32 SaveData_GetSaveRequest(void)
{
    return gSessionState[9];
}
