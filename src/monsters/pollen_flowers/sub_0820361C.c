#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void PollenFlower_UpdateAnimation(void);
void PollenFlower_Destroy(void);
s32 PollenFlower_InitializeFromScript(void *, void *);
void *PollenFlower_CreateFromScript(void *p)
{
    void *r = Task_Create(8, 0x104);
    if (r) {
        Task_SetCallbacks(r, PollenFlower_UpdateAnimation, PollenFlower_Destroy);
        if (PollenFlower_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
