#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08203DF4(void);
void sub_08204224(void);
s32 sub_08204230(void *, void *);
void *sub_08204704(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, sub_08203DF4, sub_08204224);
        if (sub_08204230(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
