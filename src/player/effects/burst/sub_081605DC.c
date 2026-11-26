#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 ActorDebris_Init(void *, void *, void *, void *);
void Task_Kill(void *);
void ActorDebris_Update(void);
void ActorDebris_Destroy(void);

void *ActorDebris_Create(void *a, void *b, void *c)
{
    void *r = Task_Create(5, 0x78);
    if (r != 0) {
        Task_SetCallbacks(r, ActorDebris_Update, ActorDebris_Destroy);
        if (ActorDebris_Init(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
