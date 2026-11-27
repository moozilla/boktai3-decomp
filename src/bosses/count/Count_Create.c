#include "bosses/count/count.h"
void *SceneActors_GetBySlot(int);
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int Count_Init(void *);
int Count_Destroy(void *);
int sub_08028F78(void *, void *);
void Task_Kill(void *);
void *Count_Create(void *arg)
{
    void *p = SceneActors_GetBySlot(0);
    if (p)
        return p;
    p = Task_Create(8, sizeof(struct Count));
    if (p) {
        Task_SetCallbacks(p, Count_Init, Count_Destroy);
        if (sub_08028F78(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
