#include "global.h"

void sub_081EC670(void *);
void Task_Kill(void *);

void SceneActors_DestroyTask(void *p)
{
    sub_081EC670(p);
    Task_Kill(p);
}
