#include "global.h"
#include "effects/healing/flicker_particle_layout.h"
struct FlickerParticlePool {
    u8 pad0[0x168];
    struct FlickerParticle particles[8];
};
s32 Callback_InvokeActor(void *, void *);
void FlickerParticle_UpdatePool(struct FlickerParticlePool *s)
{
    struct FlickerParticle *p = s->particles;
    u32 o = 0;
    s32 i = 7;
    do {
        if (p->active != 0) {
            u8 *q = (u8 *)&s->particles[0].callback;
            Callback_InvokeActor(p, *(void **)(q + o));
        }
        p++;
        o += 0x3c;
        i--;
    } while (i >= 0);
}
