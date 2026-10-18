#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_08018E00(void *);
void Task_Kill(void *);
void BlendTransition_Update(void);
void BlendTransition_Destroy(void);
void *BlendTransition_Create(void)
{
    void *obj;
    obj = Task_Create(11, 0x2C);
    if (obj != 0) {
        Task_SetCallbacks(obj, BlendTransition_Update, BlendTransition_Destroy);
        if (sub_08018E00(obj) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
