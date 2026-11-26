#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void CannonEmitter_UpdateAnimation(void);
void CannonEmitter_Destroy(void);
s32 CannonEmitter_InitializeFromScript(void *, void *);
void *CannonEmitter_CreateFromScript(void *p)
{
    void *r = Task_Create(8, 0x104);
    if (r) {
        Task_SetCallbacks(r, CannonEmitter_UpdateAnimation, CannonEmitter_Destroy);
        if (CannonEmitter_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
