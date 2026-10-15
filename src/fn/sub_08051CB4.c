// MATCHING_CONCESSION: register-pinning
#include "proximity_emitter.h"
#define ProximityEmitter_TickRise sub_08051CB4
void ProximityEmitter_TickRise(struct ProximityEmitter *input)
{
    struct ProximityEmitter *emitter = input;
    emitter->primary.sprite.flags &= ~1;
    {
        u16 *frameAddress = &emitter->stateFrame;
        register u32 previous asm("r0") = *frameAddress;
        u32 frame;
        frame = previous + 1;
        *frameAddress = frame;
        if ((u16)frame > 14) {
            emitter->primary.sprite.position.y = emitter->position.y + 30;
            {
                u32 zero = 0;
                asm("" : "+r"(zero));
                *frameAddress = zero;
            }
            emitter->update = sub_08051BD8;
        } else {
            u16 *height = (u16 *)&emitter->position.y;
            u32 rise = frame * 2;
            {
                register u32 baseHeight asm("r3") = *height;
                asm("" : "+r"(baseHeight));
                rise += baseHeight;
            }
            emitter->primary.sprite.position.y = rise;
        }
    }
    {
        u16 *address = &emitter->particleFrame;
        u32 frame = *address + 1;
        register u32 mask asm("r3");
        *address = frame;
        mask = 0xFFFF;
        asm("" : "+r"(mask));
        {
            register u32 appliedMask asm("r1") = mask;
            asm("" : "+r"(appliedMask));
            frame &= appliedMask;
        }
        frame &= 1;
        asm("" : "+r"(frame));
        if (frame == 0)
            sub_08051A8C(emitter, emitter->particleResource);
    }
}
