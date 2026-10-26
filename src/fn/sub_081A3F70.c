#include "global.h"
#include "ram.h"
void Task_Kill(void *);

void sub_081A3F70(void)
{
    Task_Kill(gEffectPoolManager);
}
