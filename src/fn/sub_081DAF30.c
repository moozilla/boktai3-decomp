#include "global.h"
s32 ResourceRegistry_GetResource(s32);
void Task_Kill(void);
s32 Task4E01_Destroy(void)
{
    s32 r;
    if (ResourceRegistry_GetResource(0x4E01)) {
        Task_Kill();
        r = 0;
    } else
        r = -1;
    return r;
}
