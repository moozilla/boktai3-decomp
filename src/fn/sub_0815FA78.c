#include "global.h"
#include "scene_vec.h"

#ifdef NONMATCHING
// fuzzy: 88.5%
// foundry draft (worker best 87.1%); what differs is mostly register allocation and instruction order
#define PlayerOrbitEffect_UpdateParticles sub_0815FA78
#define PlayerOrbitEffect_UpdateParticle  sub_0815F7F4
#define PlayerOrbitEffect_MoveParticle    PlayerBurst_UpdateParticle
struct PlayerOrbitParticle {
    u32 flags;
    u8 unknown4[0x14];
    struct SceneVec position;
    u8 unknown20[8];
    u8 active, angle, riseSpeed, expandDelay, animationPhase, angularSpeed, lifetime, state;
    u16 radius, unknown32;
    s16 velocityX, velocityY, velocityZ, unknown3A;
};
struct PlayerOrbitEffect {
    u8 unknown0[0xF8];
    void *resource;
    struct PlayerOrbitParticle particles[16];
    u16 frame;
};
void sub_0815F7F4(struct PlayerOrbitEffect *, struct PlayerOrbitParticle *);
void PlayerBurst_UpdateParticle(struct PlayerOrbitEffect *, struct PlayerOrbitParticle *);
void sub_0815FA78(struct PlayerOrbitEffect *effect)
{
    s32 remaining, i;
    struct PlayerOrbitParticle *p = effect->particles;
    u8 *active = &p->active;
    i = 0;
    remaining = 15;
    do {
        if (active[0]) {
            if (--active[6] == 0) {
                effect->particles[i].flags |= 1;
                active[0] = 0;
            } else if (active[7] == 0)
                sub_0815F7F4(effect, p);
            else
                PlayerBurst_UpdateParticle(effect, p);
        }
        p++;
        active += sizeof(*p);
        i++;
    } while (--remaining >= 0);
    effect->frame++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0815FA78);
#endif
