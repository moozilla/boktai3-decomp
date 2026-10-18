#include "global.h"
extern u32 gUnk_02000070;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 PaletteLevels_Initialize(void *, u16);
void Task_Kill(void *);
void PaletteLevels_Update(void);
void PaletteLevels_Destroy(void);

void *PaletteLevels_Create(u32 a)
{
    void *p;
    if (gUnk_02000070 != 0)
        return (void *)gUnk_02000070;
    p = Task_Create(9, 0x34);
    if (p != 0) {
        Task_SetCallbacks(p, PaletteLevels_Update, PaletteLevels_Destroy);
        if (PaletteLevels_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
