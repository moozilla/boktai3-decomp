#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 Countdown_InitializeFromScript(void *, u32, u32);
void Task_Kill(void *);
void Countdown_Tick(void);
void Countdown_Destroy(void);

void *Countdown_CreateFromScript(u32 a, u32 b)
{
    void *p = Task_Create(8, 0x1C);
    if (p) {
        Task_SetCallbacks(p, Countdown_Tick, Countdown_Destroy);
        if (Countdown_InitializeFromScript(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
