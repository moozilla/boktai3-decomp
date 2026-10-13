#include "global.h"
struct Manager {
    u8 pad[28];
    u32 flags;
};
struct Particle {
    u8 state;
    u8 pad[79];
};
struct Effect {
    u8 active, pad1, type;
    u8 pad3[5];
    struct Particle particles[4];
};
typedef s32 (*Callback)(struct Manager *, struct Effect *, s32, u32, u32, u32);
extern Callback gUnk_08604A78[][5];
s32 EffectManager_UpdateParticles(struct Manager *manager, struct Effect *effect, u32 mode)
{
    u32 one = manager->flags & 1, mask = manager->flags & 3;
    s32 i;
    struct Particle *p;
    i = 0;
    p = effect->particles;
    for (; i < 4; p++, i++)
        gUnk_08604A78[effect->type][p->state](manager, effect, i, one, mask, mode);
}
