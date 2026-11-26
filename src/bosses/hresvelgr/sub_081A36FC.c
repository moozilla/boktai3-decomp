#include "engine/shared/global.h"
#include "engine/shared/ram.h"
void Task_Kill(void *);

void sub_081A36FC(void)
{
    Task_Kill(gDirectionBurstEmitter);
}
