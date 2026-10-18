#include "global.h"
#define Callback_Invoke Callback_InvokeActor
struct ParticleVector {
    s32 x : 16, y : 16, z : 16;
    u32 : 16;
};
struct BouncingSceneParticle {
    void *update, *destroy;
    u8 unknown8[6];
    u8 phase;
    u8 unknownf[0x15];
    struct ParticleVector position;
    u8 unknown2c[8];
    struct ParticleVector velocity;
    s16 floor;
    u8 bounced;
};
s32 Callback_InvokeActor(void *, void *);
void SceneParticle_UpdateBounce(struct BouncingSceneParticle *p)
{
    u32 zero;
    p->phase += 8;
    zero = 0;
    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;
    p->velocity.y--;
    if (p->position.y < p->floor) {
        if (!p->bounced) {
            p->bounced = 1;
            p->velocity.x >>= 1;
            p->velocity.y = -(p->velocity.y >> 1);
            p->velocity.z >>= 1;
        } else {
            p->update = (void *)zero;
            if (p->destroy) {
                Callback_InvokeActor(p, p->destroy);
                p->destroy = (void *)zero;
            }
        }
    }
}
