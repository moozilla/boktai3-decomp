#include "player/effects/spiral_trail.h"
s32 sub_08249240(void *, void *, void *);
void SpiralTrail_UpdateSlots(struct SpiralTrailTask *p)
{
    if (p->emitter.active) {
        s32 count = 0;
        u8 *slot = (u8 *)&p->emitter.particles[0].active;
        struct SpiralTrailParticle *effect = p->emitter.particles;
        s32 i = 7;
        do {
            if (*slot) {
                sub_08249240(effect, p->emitter.graphics, *(void **)(slot + 0x10));
                count++;
            }
            slot += 0x3C;
            effect++;
            i--;
        } while (i >= 0);
        if (!count)
            p->emitter.active = count;
    }
}
