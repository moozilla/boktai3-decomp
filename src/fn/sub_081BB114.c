#include "global.h"
void Task_Kill(void *);

void sub_081BB114(void *task)
{
    if (task != NULL)
        Task_Kill(task);
}
