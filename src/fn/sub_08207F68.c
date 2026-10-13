#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_082074F0(void);
void sub_082079F4(void);
s32 sub_08207A00(void *, void *);
void *sub_08207F68(void *p)
{
    void *r = Task_Create(8, 0x1a8);
    if (r) {
        Task_SetCallbacks(r, sub_082074F0, sub_082079F4);
        if (sub_08207A00(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
