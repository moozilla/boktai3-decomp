#include "global.h"
#include "contact_swarm.h"
#include "ram.h"
#include "player_cue_sequence.h"
struct Particle {
    u32 age;
    u8 pad4[8];
    s16 vx, vy, vz, padE;
    u8 config[0x18];
    struct ContactSwarmSprite sprite;
    u32 flags;
    u32 pad5C;
    u32 lift;
    struct DisplaySequenceState sequence;
};
struct ParticlePool {
    u8 task[0x18];
    u32 mask;
    u8 pad1C[0x1c];
    struct Particle particles[24];
};
struct ParticleConfig {
    u8 pad0[6];
    s8 angle;
    u8 pad7;
    s32 speed;
    u8 pad0C[8];
    u32 kind;
};
extern void *const gUnk_08613F60[];
extern const s16 gUnk_086149C4[];
void *Ratatosk_CreateParticlePool(void);
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
    Memory_Copy(e->config, config, 0x18);
    e->age = 0;
    e->pad5C = 0;
    e->lift = 0;
    {
        s16 *vel = &e->vx;
        s32 angle = config->angle;
        s32 speed = config->speed;
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
        *(struct ContactPosition *)((u8 *)e + 0x48) = *position;
        {
            if (*(u32 *)((u8 *)e + 0x28) != 2)
                Anim_SetSequence(&e->sequence, *(void **)((u8 *)pool + 0xb18), 4, 0, zero);
            else
                Anim_RestartSequence(&e->sequence, *(void **)((u8 *)pool + 0xb18), 5, 0, zero);
            PlayerCue_StepSequence(&e->sprite, &e->sequence);
            e->sprite.flags &= ~1;
            Sprite_LinkToList(&e->sprite, 0);
            {
                u8 *kind = (u8 *)e + 0x33;
                *kind = 2;
            }
        }
    }
    pool->mask |= 1 << slot;
    *(u32 *)((u8 *)pool + 0xb1c) += 1;
    return slot;
}
