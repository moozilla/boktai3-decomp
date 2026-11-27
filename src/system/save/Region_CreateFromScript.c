#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
void Task_Kill(void *);
void Region_OnListUpdate(void *), Region_OnDestroy(void *);
s32 Region_InitFromScript(void *, u32, u32);
void *Region_CreateFromScript(u32 a, u32 b)
{
    void *obj = Task_Create(4, 88);
    if (obj) {
        Task_SetCallbacks(obj, Region_OnListUpdate, Region_OnDestroy);
        if (Region_InitFromScript(obj, a, b) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
