#include "global.h"
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int sub_0803AAC8(void *);
int sub_0803AAF0(void *);
int sub_0803AB28(void *);
void Task_Kill(void *);
void *sub_0803AC5C(void)
{
    void *p = Task_Create(11, 672);
    if (p) {
        Task_SetCallbacks(p, sub_0803AAC8, sub_0803AAF0);
        if (sub_0803AB28(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
