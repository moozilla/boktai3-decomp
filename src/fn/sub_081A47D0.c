#include "global.h"
#include "ram.h"
void Task_Kill(void *);

void sub_081A47D0(void)
{
    Task_Kill(gMotionEntryManager);
}
