#include "global.h"
#include "ram.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 PaletteCycle_Initialize(void *);
void Task_Kill(void *);
void PaletteCycle_Update(void);
void PaletteCycle_Destroy(void);

void *PaletteCycle_Create(void)
{
    void *p;
    if (gPaletteCycle != 0)
        return (void *)gPaletteCycle;
    p = Task_Create(0xc, 0x39c);
    if (p != 0) {
        Task_SetCallbacks(p, PaletteCycle_Update, PaletteCycle_Destroy);
        if (PaletteCycle_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
