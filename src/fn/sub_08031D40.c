#include "global.h"
#include "ram.h"
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08031840(void);
void MessageWindow_OnDestroy(void);
int sub_0803198C(void *, void *);
void *MessageWindow_Create(void *config)
{
    void *p = gMessageWindow;
    if (!p) {
        p = Task_Create(12, 720);
        if (p) {
            Task_SetCallbacks(p, sub_08031840, MessageWindow_OnDestroy);
            if (sub_0803198C(p, config) < 0) {
                Task_Kill(p);
                return 0;
            }
        }
    }
    return p;
}
