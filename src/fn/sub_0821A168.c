#include "global.h"
#include "engine.h"
/* Task_KillById: Finds a task by ID and marks it for destruction or returns -1 if absent. */
#define Task_KillById sub_0821A168

void *Task_FindById(u16);

s32 sub_0821A168(u16 id)
{
    void *t = Task_FindById(id);
    s32 r;
    if (t)
        r = Task_Kill(t);
    else
        r = -1;
    return r;
}
