#include "global.h"
#include "bosses/ratatosk/ratatosk_small_particle.h"
#include "scene/collision/contact_swarm.h"
#include "engine/ram.h"
#include "player/cutscene/player_cue_sequence.h"
extern void *const gUnk_08613F60[];
extern const s16 gUnk_086149C4[];
struct ParticlePool *Ratatosk_CreateParticlePool(void);
struct Particle *Ratatosk_FindFreeParticle(struct ParticlePool *, s32 *);
void Ratatosk_SetParticleUpdateCallback(void *, void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sprite_LinkToList(void *, s32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
s32 SceneParticles_SpawnAtPosition(struct ParticleConfig *config, struct ContactPosition *position)
{
    struct ParticlePool *pool = gParticlePool;
    struct Particle *e;
    s32 slot;
    u32 zero2;
    if (!pool) {
        Ratatosk_CreateParticlePool();
        return -1;
    }
    e = Ratatosk_FindFreeParticle(pool, &slot);
    if (!e)
        return -1;
    Ratatosk_SetParticleUpdateCallback(config, gUnk_08613F60[config->kind]);
    Memory_Copy(&e->config, config, sizeof(e->config));
    e->age = 0;
    e->fallDistance = 0;
    e->fallSpeed = 0;
    {
        s16 *vel = &e->velocity.x;
        s32 angle = config->angle;
        s32 speed = config->mode;
        s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
        s32 zero = 0;
        vel[0] = x;
        vel[1] = zero;
        {
            s32 z = Scale(gUnk_086149C4[angle & 255] * speed);
            zero2 = 0;
            vel[2] = z;
        }
    }
    {
        u32 zero = zero2;
        e->sprite.position = *position;
        if (e->config.kind != 2)
            Anim_SetSequence(&e->sequence, pool->sequenceResource, 4, 0, zero);
        else
            Anim_RestartSequence(&e->sequence, pool->sequenceResource, 5, 0, zero);
        PlayerCue_StepSequence(&e->sprite, &e->sequence);
        e->sprite.flags &= ~1;
        Sprite_LinkToList(&e->sprite, 0);
        e->sprite.priority = 2;
    }
    pool->mask |= 1 << slot;
    pool->activeCount += 1;
    return slot;
}
