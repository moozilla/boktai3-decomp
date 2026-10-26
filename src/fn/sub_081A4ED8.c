#include "global.h"
#include "ram.h"
void Task_Kill(void *);

void sub_081A4ED8(void)
{
    Task_Kill(gHresvelgrProjectilePool);
}
