#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_0800DC90(void *);
void Task_Kill(void *);
void ScanlinePresentation_Update(void);
void ScanlinePresentation_Destroy(void);

void *sub_0800E08C(void)
{
    void *p = Task_Create(8, 0x52C);
    if (p) {
        Task_SetCallbacks(p, ScanlinePresentation_Update, ScanlinePresentation_Destroy);
        if (sub_0800DC90(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
