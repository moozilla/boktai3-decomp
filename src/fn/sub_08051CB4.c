#include "proximity_emitter.h"
void ProximityEmitter_TickRise(struct ProximityEmitter *input)
{
    struct ProximityEmitter *emitter = input;
    emitter->primary.sprite.flags &= ~1;
    {
        u16 *frameAddress = &emitter->stateFrame;
        u32 previous = *frameAddress;
        u32 frame;
        frame = previous + 1;
        *frameAddress = frame;
        if ((u16)frame > 14) {
            {
                s16 *spriteY = &emitter->primary.sprite.position.y;
                *spriteY = emitter->position.y + 30;
            }
            {
                u32 zero = 0;
                *frameAddress = zero;
            }
            emitter->update = ProximityEmitter_TickActive;
        } else {
            u16 *height = (u16 *)&emitter->position.y;
            u32 rise = frame * 2;
            {
                u32 baseHeight = *height;
                rise += baseHeight;
            }
            emitter->primary.sprite.position.y = rise;
        }
    }
    if ((++emitter->particleFrame & 1) == 0)
        ProximityEmitter_EmitParticle(emitter, emitter->particleResource);
}
