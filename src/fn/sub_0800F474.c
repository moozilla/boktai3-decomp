#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 StatusScreen_Initialize(void *, u16);
void Task_Kill(void *);
void StatusScreen_Update(void);
void StatusScreen_Destroy(void);
void *sub_0800F474(u32 arg)
{
    void *obj;
    obj = Task_Create(9, 0x308);
    if (obj != 0) {
        Task_SetCallbacks(obj, StatusScreen_Update, StatusScreen_Destroy);
        if (StatusScreen_Initialize(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
