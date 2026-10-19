#define SceneBurstActor_UpdateTimedBurst sub_081B2550
#include "global.h"
#include "scene_vec.h"

struct SceneBurstActor {
    u8 pad0[0x90];
    struct SceneVec *position;
    u8 pad94[0x10];
    struct SceneVec offset;
    u8 padAC[4];
    s16 direction;
    u8 padB2[0x3E];
    u16 animation;
    u8 padF2[0x6E];
    u32 flags;
    u8 pad164[4];
    u32 timer;
    u8 pad16C[2];
    u8 reset;
    u8 pad16F;
    u8 finished;
    u8 pad171;
    u16 resetAnimation;
    u8 pad174[0x28];
    u32 state;
};
void AnimatedParticles_EmitBurst(struct SceneVec *, s32, s32, s32, s32, s32, s32, s32);
void sub_081B2550(void *unused, struct SceneBurstActor *actor)
{
    struct SceneVec point;
    struct SceneVec *out;
    struct SceneVec *base;
    struct SceneVec *offset;
    u32 mask = 0x20;
    actor->flags |= mask;
    actor->flags |= 8;
    if (actor->reset) {
        actor->reset = 0;
        actor->animation = actor->resetAnimation;
        actor->state = 0x10;
    }
    if (actor->timer <= 0x2C && !(actor->timer & 15)) {
        out = &point;
        offset = &actor->offset;
        base = actor->position;
        out->x = offset->x + base->x;
        out->y = offset->y + base->y;
        out->z = offset->z + base->z;
        AnimatedParticles_EmitBurst(out, actor->direction, 0, 0x40, 4, 4, 4, 4);
    }
    if (++actor->timer > 0x5A)
        actor->finished = 1;
}
