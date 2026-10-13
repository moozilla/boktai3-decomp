#include "global.h"
extern void *gUnk_020000DC;
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08031840(void);
void sub_08031964(void);
int sub_0803198C(void *, void *);
void *sub_08031D40(void *config)
{
    void *p = gUnk_020000DC;
    if (!p) {
        p = Task_Create(12, 720);
        if (p) {
            Task_SetCallbacks(p, sub_08031840, sub_08031964);
            if (sub_0803198C(p, config) < 0) {
                Task_Kill(p);
                return 0;
            }
        }
    }
    return p;
}
