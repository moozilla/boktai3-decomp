#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C5308(void *, void *);
void Task_Kill(void *);
void Registry1909_Update(void *);
void Registry1909_Destroy(void *);
void *SceneMessageSprite_Create(void *a)
{
    void *p = Task_Create(8, 0xdc);
    if (p != 0) {
        Task_SetCallbacks(p, Registry1909_Update, Registry1909_Destroy);
        if (sub_081C5308(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
