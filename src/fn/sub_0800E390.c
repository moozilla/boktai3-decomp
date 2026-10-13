#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 PaletteSet_Initialize(void *, u16);
void Task_Kill(void *);
void PaletteSet_Update(void);
void sub_0800E228(void);
void *sub_0800E390(u32 arg)
{
    void *obj;
    obj = Task_Create(12, 0x2C8);
    if (obj != 0) {
        Task_SetCallbacks(obj, PaletteSet_Update, sub_0800E228);
        if (PaletteSet_Initialize(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
