#include "effects/bursts/radial_burst.h"

// confidence=low: sub_08236DC4's trail setup for twelve 0x44-byte slots; frame 2 for kind 0, else 4.
void sub_082364A4(struct RadialBurst *burst)
{
    struct RadialBurstEmitter *emitter = &burst->emitter;
    s32 i;
    s32 tile;

    emitter->graphics = AnimData_FindEntry(0x1C1E);
    emitter->nextParticle = 0;
    emitter->emitDelay = 0;
    if (burst->kind == 0)
        tile = 2;
    else
        tile = 4;
    for (i = 0; i < 12; i++) {
        emitter->particles[i].active = 0;
        emitter->particles[i].tile = tile;
        ParticleSprite_Initialize(&emitter->particles[i].drawable, emitter->graphics, 1);
        Effect_SetOrigin(&emitter->particles[i].drawable, -4, -4);
        Effect_SetPalette(&emitter->particles[i].drawable, 1);
        emitter->particles[i].drawable.depthOffset = -8;
        emitter->particles[i].drawable.priority = 2;
    }
}
