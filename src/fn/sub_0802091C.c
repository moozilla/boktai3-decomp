#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 SceneObjects_Init(void *, void *, void *);
void Task_Kill(void *);
void sub_0801F93C(void);
void SceneObjects_Destroy(void);
void *sub_0802091C(void *a, void *b)
{
    void *obj;
    obj = Task_Create(9, 0x434);
    if (obj != 0) {
        Task_SetCallbacks(obj, sub_0801F93C, SceneObjects_Destroy);
        if (SceneObjects_Init(obj, a, b) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
