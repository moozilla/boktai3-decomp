#define FlickerParticle_UpdatePool sub_08235294
#include "global.h"
struct FlickerParticle {
    u32 flags;
    u8 pad4[0x18 - 4];
    u16 x, y;
    u8 pad1c[0x28 - 0x1c];
    u16 age, active, velocityX, velocityY;
    u8 pad30[4];
    void *resource;
    void *callback;
};
struct FlickerParticlePool {
    u8 pad0[0x168];
    struct FlickerParticle particles[8];
};
s32 Callback_InvokeActor(void *, void *);
void sub_08235294(struct FlickerParticlePool *s)
{
    struct FlickerParticle *p = s->particles;
    u32 o = 0;
    s32 i = 7;
    do {
        if (p->active != 0) {
            u8 *q = (u8 *)s + 0x1a0;
            Callback_InvokeActor(p, *(void **)(q + o));
        }
        p++;
        o += 0x3c;
        i--;
    } while (i >= 0);
}
