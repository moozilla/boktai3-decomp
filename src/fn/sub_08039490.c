#include "global.h"
struct Task {
    u8 pad0[0x16];
    u8 active;
};
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int sub_08039058(void *);
int sub_080390A0(void *);
int sub_080390C4(void *);
void Task_Kill(void *);
struct Task *sub_08039490(void)
{
    struct Task *p = Task_Create(2, 276);
    if (p) {
        Task_SetCallbacks(p, sub_08039058, sub_080390A0);
        if (sub_080390C4(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        p->active = 1;
    }
    return p;
}
