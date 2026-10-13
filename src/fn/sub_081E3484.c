#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 Presentation_Initialize(void *);
void Task_Kill(void *);
void Presentation_Update(void);
void Presentation_Destroy(void);
void *sub_081E3484(void)
{
    void *p = Task_Create(8, 0x238);
    if (p) {
        Task_SetCallbacks(p, Presentation_Update, Presentation_Destroy);
        if (Presentation_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
