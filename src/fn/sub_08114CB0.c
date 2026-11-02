#include "scene_particle_step.h"
static inline u32 NativeParticle_StepSprite(struct SceneParticleEffect *sprite, const u16 *graphics,
                                            struct DisplaySequenceState *state)
{
    return SceneParticle_StepSprite(state, sprite, graphics);
}
struct Particle {
    struct ContactPosition position;
    s16 dx, dy;
    u8 padC[6];
    u8 state;
    s8 index;
    u16 timer;
};
struct Manager {
    u8 pad[0x1c];
    const u16 *graphics;
    struct SceneParticleEffect sprites[32];
    struct DisplaySequenceState sequences[32];
};
void NativeParticle_UpdateAnimatedMotion(struct Manager *m, struct Particle *p)
{
    struct SceneParticleEffect *sprite = &m->sprites[p->index];
    sprite->flags &= ~1;
    p->position.x += p->dx;
    p->position.y += p->dy;
    sprite->position = p->position;
    if (NativeParticle_StepSprite(sprite, m->graphics, &m->sequences[p->index])) {
        p->state = 2;
    }
    p->timer++;
}
