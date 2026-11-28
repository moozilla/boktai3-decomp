#include "player/effects/spiral_trail.h"

void SpiralTrail_UpdateSlots(struct SpiralTrailTask *task)
{
    if (task->emitter.active) {
        s32 count = 0;
        s32 i;

        for (i = 0; i < 8; i++) {
            struct SpiralTrailParticle *particle = &task->emitter.particles[i];

            if (particle->active) {
                particle->update(particle, task->emitter.graphics);
                count++;
            }
        }
        if (!count)
            task->emitter.active = 0;
    }
}
