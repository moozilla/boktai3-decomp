#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 ResultTutorial_Initialize(void *, void *, void *);
void Task_Kill(void *);
void ResultTutorial_Finish(void);
void ResultMenu_OnTaskExit(void);
void *ResultTutorial_CreateTask(void *a, void *b)
{
    void *obj;
    obj = Task_Create(11, 0x7F4);
    if (obj != 0) {
        Task_SetCallbacks(obj, ResultTutorial_Finish, ResultMenu_OnTaskExit);
        if (ResultTutorial_Initialize(obj, a, b) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
