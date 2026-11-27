#include "player/effects/spiral_trail.h"
#include "player/player.h"


void sub_08236F48(struct SpiralTrailTask *p)
{
    if (p->sprite.active) {
        u32 limit = p->fadeLimit;
        u32 frame = p->timer;
        u16 storedLimit = limit;
        if (frame < limit && p->parentMode != p->parent->tetherMode)
            p->timer = storedLimit;
        frame = ++p->timer;
        if ((frame << 16) < ((u32)storedLimit << 16)) {
            SpiralTrail_AdvanceFrame(&p->sprite);
            p->sprite.drawable.position = *(const struct ContactPosition *)&p->parent->position;
            p->sprite.drawable.position.x += p->offset.x;
            p->sprite.drawable.position.y += p->offset.y;
            p->sprite.drawable.position.z += p->offset.z;
            if (p->mode == 1 && p->timer < p->emissionLimit)
                SpiralTrail_EmitOrbitParticle(p, &p->sprite.drawable.position);
        } else {
            SpiralTrail_RewindFrame(&p->sprite);
        }
    }
}
