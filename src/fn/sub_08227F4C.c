#include "global.h"
#include "runtime.h"
/* SaveData_GetSaveRequest: Returns header byte 9 paired with the tentative save request setter */
#define SaveData_GetSaveRequest sub_08227F4C
u32 sub_08227F4C(void)
{
    return gSessionState[9];
}
