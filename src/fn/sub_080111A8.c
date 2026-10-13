#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 GroupObjects_Initialize(void *, void *);
void Task_Kill(void *);
void GroupObjects_Update(void);
void GroupObjects_Destroy(void);
void *sub_080111A8(void *arg)
{
    void *obj;
    obj = Task_Create(11, 0xD08);
    if (obj != 0) {
        Task_SetCallbacks(obj, GroupObjects_Update, GroupObjects_Destroy);
        if (GroupObjects_Initialize(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
