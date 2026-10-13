#include "global.h"
void *sub_0802140C(int);
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int sub_08028F24(void *);
int sub_08028F48(void *);
int sub_08028F78(void *, void *);
void Task_Kill(void *);
void *sub_08029798(void *arg)
{
    void *p = sub_0802140C(0);
    if (p)
        return p;
    p = Task_Create(8, 0xa10);
    if (p) {
        Task_SetCallbacks(p, sub_08028F24, sub_08028F48);
        if (sub_08028F78(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
