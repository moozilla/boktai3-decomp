#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void PaletteFrames_Update(void);
void PaletteFrames_Destroy(void);
s32 PaletteFrames_Initialize(void *);
void *PaletteFrames_Create(void)
{
    void *r = Task_Create(12, 0x220);
    if (r) {
        Task_SetCallbacks(r, PaletteFrames_Update, PaletteFrames_Destroy);
        if (PaletteFrames_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
