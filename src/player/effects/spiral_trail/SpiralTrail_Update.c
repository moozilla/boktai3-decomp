#include "player/effects/spiral_trail.h"
void Task_Kill(void *);
u32 SpiralTrail_Update(struct SpiralTrailTask *p)
{
    sub_08236F48(p);
    if (p->mode == 1)
        SpiralTrail_UpdateSlots(p);
    if (!p->sprite.active && !p->emitter.active)
        Task_Kill(p);
}
