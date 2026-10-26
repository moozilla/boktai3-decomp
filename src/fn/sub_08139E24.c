#define SpritePool_Create           ParticleBurstEffect_EnsureManager
#define SpritePoolEntry_SetCallback ParticleBurstEffect_SetStateCallback
#define gSpritePool                 gSceneProcParticles
#include "global.h"
#include "ram.h"
struct SpritePoolVector {
    u32 v[4];
};
struct SpritePoolEntry {
    u8 pad0[0x8c];
    u8 anim[0x10];
    void *data;
    struct SpritePoolVector position;
    u8 padB0[0x10];
    void *owner;
    u8 padC4[4];
    u16 duration;
    u8 active;
    u8 padCB;
    u16 id;
    u8 padCE;
    u8 state;
    u32 value;
    u8 padD4[4];
};
struct SpritePool {
    u8 pad[0x18];
    struct SpritePoolEntry entries[6];
    u32 counter;
};
struct SpritePoolParams {
    u16 unk0, duration, unk4, value;
};
struct SpritePool *ParticleBurstEffect_EnsureManager(void);
u8 SceneProcParticles_TestCapacity(struct SpritePool *, u32, u32);
void SceneProcParticles_SubmitBounds(struct SpritePoolEntry *, void (*)(void), u32, u32);
void ParticleBurstEffect_SetStateCallback(struct SpritePoolEntry *, void (*)(void));
void ParticleBurstEffect_OnAnimationComplete(void);
void ParticleBurstEffect_UpdateFalling(void);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
static inline u32 SpritePool_MakeKey(u32 counter, u32 id)
{
    u32 mask = 0xff;
    id &= mask;
    counter &= mask;
    counter <<= 8;
    id |= counter;
    return id;
}
s32 SpritePool_StartEntry(struct SpritePoolVector *position, void *owner, u32 unused,
                          struct SpritePoolParams *params, u32 id)
{
    struct SpritePool *pool;
    s32 i;
    u32 key;
    if (!gSceneProcParticles) {
        pool = ParticleBurstEffect_EnsureManager();
        if (!pool)
            return -1;
    } else
        pool = gSceneProcParticles;
    key = pool->counter;
    key = SpritePool_MakeKey(key, id);
    if (SceneProcParticles_TestCapacity(pool, key, 1))
        return -1;
    for (i = 0; i < 6; i++) {
        if (!pool->entries[i].active) {
            pool->entries[i].position = *position;
            pool->entries[i].owner = owner;
            pool->entries[i].duration = params->duration;
            pool->entries[i].active = 1;
            pool->entries[i].state = 0;
            pool->entries[i].value = 0;
            key = pool->counter;
            key = SpritePool_MakeKey(key, id);
            pool->entries[i].id = key;
            Anim_SetSequence(pool->entries[i].anim, pool->entries[i].data, 7, 0, 0);
            SceneProcParticles_SubmitBounds(
                &pool->entries[i], ParticleBurstEffect_OnAnimationComplete, params->value, id + 1);
            ParticleBurstEffect_SetStateCallback(&pool->entries[i],
                                                 ParticleBurstEffect_UpdateFalling);
            Sound_PlaySong(0x3e5);
            return i;
        }
    }
    return -1;
}
