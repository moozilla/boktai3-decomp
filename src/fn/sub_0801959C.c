#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void PaletteFrames_Update(void);
void sub_0801958C(void);
s32 sub_08019590(void *);
void *sub_0801959C(void)
{
    void *r = Task_Create(12, 0x220);
    if (r) {
        Task_SetCallbacks(r, PaletteFrames_Update, sub_0801958C);
        if (sub_08019590(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
