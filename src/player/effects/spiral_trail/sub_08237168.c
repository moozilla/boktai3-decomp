#include "global.h"
void *Task_Create(u32, u32);
void Task_Kill(void *);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void SpiralTrail_Update(void);
void SpiralTrail_Destroy(void);
s32 ActorAttachment_Initialize(void *, void *, u32, void *, u32, u32, u32);

void *ActorAttachment_Create(void *parent, u32 kind, void *offset, u32 a, u32 b, u32 mode)
{
    void *p = Task_Create(8, 0x288);
    if (p) {
        Task_SetCallbacks(p, SpiralTrail_Update, SpiralTrail_Destroy);
        if (ActorAttachment_Initialize(p, parent, kind, offset, a, b, mode) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
