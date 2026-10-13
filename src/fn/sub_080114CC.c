#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 MessageMenu_Initialize(void *);
void Task_Kill(void *);
void MessageMenu_Update(void);
void MessageMenu_Destroy(void);
void *sub_080114CC(void)
{
    void *obj;
    obj = Task_Create(11, 0x4D8);
    if (obj != 0) {
        Task_SetCallbacks(obj, MessageMenu_Update, MessageMenu_Destroy);
        if (MessageMenu_Initialize(obj) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
