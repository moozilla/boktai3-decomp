#include "global.h"
#include "ram.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_08163700(void *);
void Task_Kill(void *);
void GameOver_OnUpdate(void);
void GameOver_OnDestroy(void);

void *GameOver_Create(void)
{
    void *r = gGameOver;
    void *q;
    if (r == 0) {
        q = Task_Create(0xb, 0x354);
        if (q != 0) {
            Task_SetCallbacks(q, GameOver_OnUpdate, GameOver_OnDestroy);
            if (sub_08163700(q) < 0) {
                Task_Kill(q);
                return 0;
            }
        }
        r = q;
    }
    return r;
}
