#include "global.h"
void *SceneActors_GetBySlot(int);
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int Vampire_Init(void *);
int Vampire_Destroy(void *);
int sub_08028F78(void *, void *);
void Task_Kill(void *);
void *Vampire_Create(void *arg)
{
    void *p = SceneActors_GetBySlot(0);
    if (p)
        return p;
    p = Task_Create(8, 0xa10);
    if (p) {
        Task_SetCallbacks(p, Vampire_Init, Vampire_Destroy);
        if (sub_08028F78(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
