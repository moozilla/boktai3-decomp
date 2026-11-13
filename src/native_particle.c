#include "native_particle.h"
#include "global.h"
#include "ram.h"
#include "scene_particle_step.h"
#include "game_state.h"
#include "render.h"
#include "engine.h"

/* NativeEffect_ResetManager: Clears the native effect singleton pointer */

void NativeEffect_ResetManager(void)
{
    gSceneCallbackPool = 0;
}

/* NativeEffect_IgnoreUpdate: Empty hook beside the native effect manager reset routine with tentative lifecycle role */

void NativeEffect_IgnoreUpdate(void)
{
}

static inline u32 NativeParticle_StepSprite(struct SceneParticleEffect *sprite, const u16 *graphics,
                                            struct DisplaySequenceState *state)
{
    return SceneParticle_StepSprite(state, sprite, graphics);
}

void NativeParticle_UpdateAnimatedMotion(struct ParticleManager *manager,
                                         struct NativeParticle *particle)
{
    struct SceneParticleEffect *sprite = &manager->sprites[particle->control.index];
    sprite->flags &= ~1;
    particle->position.x += particle->velocity.x;
    particle->position.y += particle->velocity.y;
    sprite->position = particle->position;
    if (NativeParticle_StepSprite(sprite, manager->graphics,
                                  &manager->sequences[particle->control.index])) {
        particle->control.state = 2;
    }
    particle->control.timer++;
}

void Effect_Remove(void *);

void NativeParticle_ReleaseEntry(struct ParticleManager *manager, struct NativeParticle *particle)
{
    Effect_Remove(&manager->sprites[particle->control.index]);
    particle->control.state = 0;
    particle->control.active = 0;
    Memory_Clear(particle, sizeof(*particle));
    particle->control.index = 0xff;
}

struct NativeParticle *NativeParticle_AllocateEntry(struct ParticleManager *manager)
{
    s32 index = 0;
    u32 one = 1;
    u32 *active = &manager->entries[0].control.active;
    struct NativeParticle *particle = manager->entries;
    struct NativeParticle *result;
    do {
        if (*active != 1) {
            particle->control.index = index;
            *active = one;
            result = particle;
            goto end;
        }
        active = (u32 *)((u8 *)active + sizeof(*particle));
        particle = particle + 1;
        index++;
    } while (index <= 0x1f);
    result = 0;
end:
    return result;
}

extern const u16 gUnk_086141BC[];

s32 Mod(s32, s32);

void ParticleSprite_Initialize(void *, void *, u32);

void Effect_SetOrigin(void *, s32, s32);

static inline u16 NativeParticle_NextRandom(u32 *seed)
{
    *seed = (*seed + 1) & 1023;
    return gUnk_086141BC[*seed];
}

static inline u8 NativeParticle_IsNight(void)
{
    u32 mask = 8;
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}

s32 NativeParticle_CreateBatch(struct ContactPosition *position, s16 *range, s16 *spread, s32 count,
                               u32 priority, u32 animFlags, u32 spriteFlags, u32 tag)
{
    struct ParticleManager *manager;
    struct SceneParticleEffect *sprite;
    s32 i;
    if (!position)
        return -1;
    manager = gParticleManager;
    if ((!manager) && (!(manager = EffectPool_CreateManager())))
        return -1;
    for (i = 0; i < count; i++) {
        struct NativeParticle *particle = NativeParticle_AllocateEntry(manager);
        s8 sequence;
        u32 flag;
        if (!particle)
            return -1;
        sprite = &manager->sprites[particle->control.index];
        {
            typeof(particle->position) *destination = &particle->position;
            *destination = *position;
            if (spread) {
                if (spread[0])
                    particle->position.x +=
                        Mod(NativeParticle_NextRandom(&gEffectRandomTableIndex), spread[0] * 2) -
                        spread[0];
                if (spread[1])
                    particle->position.y +=
                        Mod(NativeParticle_NextRandom(&gEffectRandomTableIndex), spread[1] * 2) -
                        spread[1];
            }
            if (!range) {
                particle->velocity.x = 0;
                particle->velocity.y = 0;
            } else {
                struct ContactPosition v;
                if (range[0] == 0)
                    v.x = 0;
                else
                    v.x = Mod(NativeParticle_NextRandom(&gEffectRandomTableIndex), range[0]);
                if (range[1] == 0)
                    v.y = 0;
                else
                    v.y = Mod(NativeParticle_NextRandom(&gEffectRandomTableIndex), range[1]);
                particle->velocity = v;
            }
            ParticleSprite_Initialize(sprite, manager->graphics, 0);
            Effect_SetOrigin(sprite, -8, -8);
            flag = NativeParticle_IsNight();
            sequence = 13;
            if (flag)
                sequence = 14;
            Anim_SetSequence(
                &manager->sequences[particle->control.index], manager->resource, sequence, 0,
                (u8)(Mod(NativeParticle_NextRandom(&gEffectRandomTableIndex), 2) | animFlags));
            sprite->flags |= 0x11 | spriteFlags;
            sprite->priority = priority;
            particle->control.state = 1;
            particle->control.flags = tag;
        }
    }

    return 0;
}

u32 NativeParticle_CreateWithFlag(u32 position, u32 range, u32 spread, u32 count, u32 priority,
                                  u32 spriteFlags, u32 animationFlags)
{
    return NativeParticle_CreateBatch((void *)position, (void *)range, (void *)spread, count,
                                      priority, animationFlags, spriteFlags, 1);
}

s32 NativeParticle_CreateWithoutFlag(u32 position, u32 range, u32 spread, u32 count, u32 priority,
                                     u32 spriteFlags, u32 animationFlags)
{
    return NativeParticle_CreateBatch((void *)position, (void *)range, (void *)spread, count,
                                      priority, animationFlags, spriteFlags, 0);
}

extern void *gUnk_086106B4[];

s32 sub_08249240(void *, void *, void *);

s32 EffectPool_UpdateEntries(struct ParticleManager *pool)
{
    u32 mask = 0x200;
    struct NativeParticleState *state = &pool->entries[0].control;
    struct NativeParticle *entry = pool->entries;
    s32 i = 31;
    do {
        if (state->active) {
            if (!(((gEngineFlags | gScenePauseFlags) & mask) ||
                  ((gEngineFlags & 0x800) && (gEnginePauseFlags & 7))) ||
                !(state->flags & 1))
                sub_08249240(pool, entry, gUnk_086106B4[state->state]);
        }
        state = (void *)((u8 *)state + sizeof(*entry));
        entry++;
    } while (--i >= 0);
    return 0;
}

s32 NativeParticle_DestroyManager(struct ParticleManager *manager)
{
    struct NativeParticle *particle = manager->entries;
    s32 index = 0x1f;
    do {
        if (particle->control.active != 0)
            Effect_Remove(&manager->sprites[particle->control.index]);
        particle = particle + 1;
        index--;
    } while (index >= 0);
    {
        u32 z = 0;
        gParticleManager = (void *)z;
    }
    return 0;
}

void NativeParticle_LoadResources(struct ParticleManager *manager)
{
    manager->graphics = AnimData_FindEntry(0x1C1A);
    manager->resource = ScriptProc_Start(0x922E, 0xD1B8);
}

s32 EffectPool_InitManager(struct ParticleManager *manager)
{
    NativeParticle_LoadResources(manager);
    gParticleManager = manager;
    return 0;
}

struct ParticleManager *EffectPool_CreateManager(void)
{
    struct ParticleManager *manager;
    if (gParticleManager != 0)
        return gParticleManager;
    manager = Task_Create(0xa, sizeof(*manager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void (*)(void *))EffectPool_UpdateEntries,
                          (void (*)(void *))NativeParticle_DestroyManager);
        if (EffectPool_InitManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}
