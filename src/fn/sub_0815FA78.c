#include "global.h"
#include "scene_vec.h"

#define PlayerOrbitEffect_MoveParticle PlayerBurst_UpdateParticle
struct PlayerOrbitParticleMotion {
    u8 active, angle, riseSpeed, expandDelay, animationPhase, angularSpeed, lifetime, state;
    u16 radius, unknown32;
    s16 velocityX, velocityY, velocityZ, unknown3A;
};
struct PlayerOrbitParticle {
    u32 flags;
    u8 unknown4[0x14];
    struct SceneVec position;
    u8 unknown20[8];
    struct PlayerOrbitParticleMotion motion;
};
struct PlayerOrbitEffect {
    u8 unknown0[0xF8];
    void *resource;
    struct PlayerOrbitParticle particles[16];
    u16 frame;
};
void PlayerOrbitEffect_UpdateParticle(struct PlayerOrbitEffect *, struct PlayerOrbitParticle *);
void PlayerBurst_UpdateParticle(struct PlayerOrbitEffect *, struct PlayerOrbitParticle *);
void PlayerOrbitEffect_UpdateParticles(struct PlayerOrbitEffect *effect)
{
    s32 i;
    struct PlayerOrbitParticle *p = effect->particles;
    struct PlayerOrbitParticleMotion *motion = &p->motion;
    i = 0;
    do {
        if (motion->active) {
            if ((--motion->lifetime) == 0) {
                effect->particles[i].flags |= 1;
                motion->active = 0;
            } else if (motion->state == 0)
                PlayerOrbitEffect_UpdateParticle(effect, p);
            else
                PlayerBurst_UpdateParticle(effect, p);
        }
        p++;
        motion = (struct PlayerOrbitParticleMotion *)((u8 *)motion + sizeof(*p));
        i++;
    } while (i < 16);
    effect->frame++;
}
