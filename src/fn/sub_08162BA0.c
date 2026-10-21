#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 SceneMarker_InitSprites(void *, void *);
void Task_Kill(void *);
void PlayerBarrier_Update(void);
void PlayerBarrier_Destroy(void);

void *PlayerBarrier_Create(void *a)
{
    void *r = Task_Create(8, 0x150);
    if (r != 0) {
        Task_SetCallbacks(r, PlayerBarrier_Update, PlayerBarrier_Destroy);
        if (SceneMarker_InitSprites(r, a) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
