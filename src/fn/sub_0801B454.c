#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 SelectionMenu_Initialize(void *, void *, void *);
void Task_Kill(void *);
void sub_0801B2D4(void);
void sub_0801B2F8(void);
void *sub_0801B454(void *a, void *b)
{
    void *obj;
    obj = Task_Create(11, 0xDB4);
    if (obj != 0) {
        Task_SetCallbacks(obj, sub_0801B2D4, sub_0801B2F8);
        if (SelectionMenu_Initialize(obj, a, b) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
