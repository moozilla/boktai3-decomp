#include "global.h"
s32 sub_0821A2C4(s32);
void Task_Kill(void);
s32 sub_081DAF30(void)
{
    s32 r;
    if (sub_0821A2C4(0x4E01)) {
        Task_Kill();
        r = 0;
    } else
        r = -1;
    return r;
}
