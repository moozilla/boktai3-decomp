#include "region_actor.h"
#include "player_cue_sequence.h"
#include "ram.h"

void AnimatedParticles_EmitChargeSparkle(s32, s32, s32, s32, void *);

void Sound_PlaySong(u32);

void sub_08249244(void *, void *, void *, void *);

static inline u32 Mask(u32 *p, u32 mask)
{
    return *p & mask;
}

// confidence=low: collider callback applies damage and emits contact sparkles for both variants.

void ProximityActor_ApplyContactDamage(struct ContactCollider *a, struct ContactCollider *b,
                                       struct RegionActor *p)
{
    u32 blocked;
    if (RegionActor_HasBehavior(p, 8))
        return;
    if (p->variant == 1)
        blocked = RegionActor_HasBehavior(p, 2);
    else if (Mask(&a->statRewardFlags, 16))
        blocked = 1;
    else
        blocked = RegionActor_HasBehavior(p, 2);
    if (blocked) {
        b->cost = 0;
        b->responseParameter = 0;
        return;
    }
    if (Mask(&a->reactionFlags, 8)) {
        b->costFlags = 1;
        if (p->variant == 0) {
            if (p->mode)
                p->mode--;
            else {
                AnimatedParticles_EmitChargeSparkle(2, 0, 0, 64, &p->origin);
                p->mode = 3;
                Sound_PlaySong(348);
            }
        } else if (p->variant == 1) {
            if (p->mode)
                p->mode--;
            else {
                AnimatedParticles_EmitChargeSparkle(2, 2, 0, 64, &p->origin);
                p->mode = 2;
                Sound_PlaySong(348);
            }
        }
        if (RegionActor_HasBehavior(p, 4)) {
            if (p->variant == 1) {
                p->health -= 200;
                p->health = p->health < 0 ? 0 : p->health;
            } else {
                p->health -= a->cost;
                p->health = p->health < 0 ? 0 : p->health;
            }
        }
    }
    if (p->variant == 1) {
        void *data = a->owner;
        if (data) {
            void *callback = a->callback;
            if (callback && (Mask(&a->statRewardFlags, 16)))
                sub_08249244(a, b, data, callback);
        }
    }
    return;
}

void ProximityActor_InitializeCollider(struct RegionActor *actor, s16 width, s16 height)
{
    struct RegionActorColliderVector size, offset;
    if (actor->variant == 1) {
        size.x = 0x56;
        size.y = 0x80;
        size.z = 0x56;
        offset.x = 0;
        offset.y = 0x80;
        offset.z = 0;
    } else {
        size.x = 0x60;
        size.y = 0x10;
        size.z = 0x60;
        offset.x = 0;
        offset.y = 0x10;
        offset.z = 0;
    }
    Collider_Init(&actor->collider, 0, 0x4003, 0, 16, &size, &offset);
    Collider_SetPosition(&actor->collider, &actor->origin, 0);
    Collider_SetCallback(&actor->collider, ProximityActor_ApplyContactDamage, actor);
    Collider_Refresh(&actor->collider);
}

/* Pool128_OnStateNoOp: Empty function preceding the 0x128-byte pool allocator; state callback role is tentative */

void Pool128_OnStateNoOp(void)
{
}

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

/* Takes the first free one of the pool's 32 slots: a cleared 0x128-byte block
   recorded at +0x20 with its index at +0x104; NULL when full. */

struct RegionActor *Pool128_Allocate(struct RegionSpriteManager *pool)
{
    s32 i;
    for (i = 0; i < 32; i++) {
        if (!Tst(&pool->used, i)) {
            struct RegionActor *actor = Heap_Alloc(sizeof(*actor));
            Memory_Clear(actor, sizeof(*actor));
            pool->used |= 1 << i;
            actor->resourceId = i;
            pool->actors[i] = actor;
            return actor;
        }
    }
    return 0;
}

void Anim_RestartSequence(struct DisplaySequenceState *, void *, u32, u32, u32);

s32 Div(s32, s32);

static inline void Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st)
{
    PlayerCue_StepSequence(sp, st);
}

// confidence=low: health-derived animation and recovery; actor identity remains tentative.

void ProximityActor_UpdateDamageAnimation(struct RegionActor *p)
{
    struct RegionSpriteManager *ctx = gRegionActorPool;
    s32 frame;
    if (!ctx)
        return;
    if ((p->collider.costFlags & 1) || RegionActor_HasBehavior(p, 4)) {
        if (p->health <= 0) {
            p->behaviorFlags = 0x28;
            Anim_SetSequence(&p->sequence, ctx->sequenceResource, 1, 0, 0);
            Step(&p->sprite, &p->sequence);
            p->kind = 3;
            p->entered = 1;
            p->collider.costFlags = 0;
            return;
        } else {
            u32 test = RegionActor_HasBehavior(p, 1);
            if (test) {
                if (p->sequence.index != 3) {
                    frame = p->sequence.index;
                    if (frame > 3)
                        frame = __builtin_abs(frame - 6);
                    Anim_RestartSequence(&p->sequence, ctx->sequenceResource, 0, 0, 0);
                    p->sequence.index = frame;
                }
                p->behaviorFlags = 0x22;
                return;
            } else {
                p->behaviorFlags = 0x24;
                frame = Div(p->health, p->threshold);
                frame = 3 - frame;
                Anim_SetSequence(&p->sequence, ctx->sequenceResource, 1, 0, test);
                if (p->sequence.index < frame)
                    Step(&p->sprite, &p->sequence);
            }
        }
    }
    if (!(p->collider.costFlags & 1)) {
        if (p->health < p->maxHealth)
            p->health += 10;
        else if (RegionActor_HasBehavior(p, 4)) {
            Anim_SetSequence(&p->sequence, ctx->sequenceResource, 1, 0, 4);
            if (RegionActor_HasBehavior(p, 4)) {
                frame = Div(p->health, p->threshold);
                p->sequence.index = 3 - frame;
            }
            p->health = p->maxHealth;
            p->behaviorFlags = 0x30;
        }
        p->mode = 0;
    }
    p->collider.costFlags = 0;
}

void RegionActor_UpdateBehaviorAnimation(struct RegionActor *input)
{
    struct RegionActor *p = input;
    struct ContactSwarmSprite *sprite = &input->sprite;
    struct RegionSpriteManager *manager = gRegionActorPool;
    if (manager) {
        p->sprite.flags &= ~1;
        if (RegionActor_HasBehavior(p, 0x10)) {
            if (PlayerCue_StepSequenceEnded(sprite, &p->sequence)) {
                Anim_SetSequence(&p->sequence, manager->sequenceResource, 0, 0, 0);
                p->sequence.index = 3;
                p->behaviorFlags = 0x21;
            }
        } else if (RegionActor_HasBehavior(p, 2)) {
            PlayerCue_StepSequence(sprite, &p->sequence);
            if (p->sequence.index == 3)
                p->behaviorFlags = 0x24;
        } else {
            ProximityActor_UpdateDamageAnimation(p);
            if (RegionActor_HasBehavior(p, 1))
                PlayerCue_StepSequence(&p->sprite, &p->sequence);
        }
    }
}

static inline u8 RegionActor_TestFlagWord(u32 *flags, u32 bit)
{
    if (*flags & bit)
        return 1;
    return 0;
}

// confidence=low: updates animation, enters the death state on depleted health, otherwise restores ten health.

void RegionActor_UpdateHealthAnimation(struct RegionActor *p)
{
    struct ContactSwarmSprite *sprite = &p->sprite;
    sprite->flags &= ~1;
    if (gRegionActorPool) {
        PlayerCue_StepSequenceEnded(sprite, &p->sequence);
        if (p->collider.costFlags & 1) {
            if (p->health <= 0) {
                p->behaviorFlags = 0x28;
                p->kind = 4;
                p->entered = 1;
            } else
                p->collider.costFlags = 0;
        } else if (p->health < p->maxHealth)
            p->health += 10;
    }
}

static inline u8 TestV(u32 flags, u32 bit)
{
    if (flags & bit)
        return 1;
    return 0;
}

// confidence=low: in kinds 3-4, releases the registered terrain cell once (behavior flag 0x20).

void RegionActor_UnregisterTerrain(struct RegionActor *p)
{
    if ((u16)(p->kind - 3) <= 1) {
        u32 m = 0x20;
        u32 *fp = &p->behaviorFlags;
        u32 v = *fp & m;
        u32 *flags = fp;
        if (TestV(v, ~0)) {
            TerrainModifiers_UnregisterCell(&p->terrainCell);
            {
                u32 c = ~0x20;
                *flags &= c;
            }
        }
    }
}

union SparkPosition {
    struct ContactPosition position;
    struct {
        u32 x : 16, y : 16, z : 16, pad : 16;
    } packed;
};

// confidence=low: emits periodic sparkles around the origin while the transition animation plays.

void RegionActor_AnimateSparkleTransition(struct RegionActor *p)
{
    struct RegionSpriteManager *manager = gRegionActorPool;
    u32 done;
    if (manager) {
        if (p->entered) {
            RegionActor_UnregisterTerrain(p);
            {
                p->frames = 0;
                p->entered = 0;
            }
        }
        done = PlayerCue_StepSequenceEnded(&p->sprite, &p->sequence);
        if (p->frames <= 31) {
            union SparkPosition position;
            if (Mod(p->frames, 6) == 0) {
                position.position = p->origin;
                position.packed.x -= 64;
                {
                    union SparkPosition *output = &position;
                    AnimatedParticles_EmitChargeSparkle(1, 0, 0, 64, output);
                }
            }
            if (Mod(p->frames, 10) == 0) {
                position.position = p->origin;
                position.position.x += 64;
                AnimatedParticles_EmitChargeSparkle(1, 0, 0, 64, &position);
            }
            if (Mod(p->frames, 16) == 0) {
                position.position = p->origin;
                position.position.z += 32;
                AnimatedParticles_EmitChargeSparkle(1, 0, 0, 64, &position);
            }
            if (Mod(p->frames, 7) == 0) {
                s8 *sound = &manager->cursor;
                if (*sound < 0) {
                    Sound_PlaySong(0x15C);
                    *sound = 1;
                }
            }
            p->frames++;
        } else if (done)
            p->kind = 0;
    }
}

// confidence=low: entered flag selects the final alternate animation and emits charge sparkles.

void RegionActor_AnimateRemoval(struct RegionActor *p)
{
    struct RegionSpriteManager *manager = gRegionActorPool;
    if (manager) {
        if (p->entered) {
            RegionActor_UnregisterTerrain(p);
            Anim_SetSequence(&p->sequence, manager->alternateResource, 4, 0, 0);
            PlayerCue_StepSequence(&p->sprite, &p->sequence);
            p->frames = 0;
            p->entered = 0;
            AnimatedParticles_EmitChargeSparkle(1, 2, 0, 64, &p->sprite.position);
        }
        if (PlayerCue_StepSequenceEnded(&p->sprite, &p->sequence)) {
            p->kind = 0;
            p->sprite.flags |= 1;
            AnimatedParticles_EmitChargeSparkle(3, 2, 0, 64, &p->sprite.position);
        }
    }
}

s32 Region_FindContaining(void *);

// confidence=low: script-created region actor selects one of two sprite resources and registers its terrain cell.

void RegionActor_CreateFromScript(void)
{
    struct RegionSpriteManager *manager = gRegionActorPool;
    struct RegionActor *p;
    s32 width, height, x, z, cell;
    struct ContactSwarmSprite *sprite;
    u32 one;
    u32 *flags;
    if (!manager)
        return;
    p = Pool128_Allocate(manager);
    if (!p)
        return;
    p->behaviorFlags = 1;
    if (Script_SeekToKeyword('p')) {
        p->origin.x = Script_GetValue();
        p->origin.y = Script_GetValue();
        p->origin.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        width = Script_GetValue();
        height = Script_GetValue();
    } else {
        width = 0;
        height = 0;
    }
    p->health = Script_GetKeywordValue('h', 0);
    p->maxHealth = p->health;
    sprite = &p->sprite;
    sprite->position = p->origin;
    Sprite_Init(p, &p->graphics, 0);
    PlayerCue_StepSequenceEnded(sprite, &p->sequence);
    {
        struct ContactSwarmSprite *s = sprite;
        u32 f = s->flags;
        one = 1;
        s->flags = f | one;
    }
    p->variant = Script_GetKeywordValue('m', 0);
    switch (p->variant) {
        case 0:
            AnimData_Load(&p->graphics, 0x4FDA);
            Anim_SetSequence(&p->sequence, manager->sequenceResource, 0, 0, 0);
            p->kind = one;
            sprite->priority = 3;
            flags = &p->behaviorFlags;
            break;
        case 1:
            AnimData_Load(&p->graphics, 0x863A);
            Anim_SetSequence(&p->sequence, manager->alternateResource, 2, 0, 0);
            p->kind = 2;
            {
                u32 bit = 4;
                p->behaviorFlags |= bit;
            }
            sprite->priority = 2;
            flags = &p->behaviorFlags;
            break;
        default:
            flags = &p->behaviorFlags;
            break;
    }
    p->mode = 0;
    p->threshold = Div(p->health, 3);
    ProximityActor_InitializeCollider(p, width, height);
    x = p->sprite.position.x >> 8;
    z = p->sprite.position.z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    TerrainModifiers_RegisterCell(&p->terrainCell, cell, 0, p->sprite.position.y >> 8, 0, 128);
    {
        u32 bit = 0x20;
        *flags |= bit;
    }
    if (Script_GetKeywordValue('P', 0))
        *flags |= 0x80;
    p->region = Region_FindContaining(&p->origin);
    if (Script_GetKeywordValue('s', 0)) {
        p->overlayActive = 1;
        SceneBody_Init(&p->body, p->resourceId + 0x6F, &p->origin, 0, 7, p);
        SceneBody_SetSprite(&p->body, p);
        TerrainOverlay_QueueAddActor(&p->body);
    } else
        p->overlayActive = 0;
}

#define gRegionActorUpdateCallbacks gUnk_086064FC

extern void *gUnk_086064FC[];

s32 Callback_InvokeActor(void *, void *);

static inline u8 RegionActorPool_TestSlotBit(u32 *used, u32 mask)
{
    if (*used & mask)
        return 1;
    return 0;
}

static inline u8 TestFlags(struct RegionActor *actor)
{
    u32 mask = 0x80;
    if (actor->behaviorFlags & mask)
        return 1;
    return 0;
}

static inline u8 IsCurrentRegion(struct RegionActor *actor)
{
    s32 region = actor->region;
    if (gGameState->region == region)
        return 1;
    return 0;
}

s32 RegionActorPool_UpdateActors(struct RegionSpriteManager *pool)
{
    struct RegionActor **actors = pool->actors;
    s32 i;
    void **callbacks;
    u32 *used;
    u8 *cursor;
    if (pool->used) {
        i = 0;
        used = &pool->used;
        cursor = (u8 *)&pool->cursor;
        callbacks = gUnk_086064FC;
        do {
            if (RegionActorPool_TestSlotBit(used, 1U << i)) {
                if (TestFlags(actors[i]) && !IsCurrentRegion(actors[i]))
                    actors[i]->sprite.flags |= 1;
                else
                    Callback_InvokeActor(actors[i], callbacks[actors[i]->kind]);
            }
            i++;
        } while (i < 32);
        *cursor = 0xFF;
    }
    return 0;
}

void Sprite_Remove(void *);

void Heap_Free(void *);

static inline u8 RegionActorPool_HasSlot(struct RegionSpriteManager *pool, s32 index)
{
    u32 *used = &pool->used;
    u32 mask = 1U << index;
    if (*used & mask)
        return 1;
    return 0;
}

static inline u8 TestFlagsValue(u32 v)
{
    if (v)
        return 1;
    return 0;
}

s32 RegionActorPool_DestroyActors(struct RegionSpriteManager *pool)
{
    s32 i;
    for (i = 0; i < 32; i++) {
        if (RegionActorPool_HasSlot(pool, i)) {
            struct RegionActor *actor = pool->actors[i];
            Sprite_Remove(actor);
            if (actor->overlayActive) {
                TerrainOverlay_QueueRemoveActor(&actor->body);
                SceneBody_Unregister(&actor->body);
            }
            {
                u32 m20 = 0x20;
                u32 *flags;
                u32 *t;
                u32 v;
                t = &actor->behaviorFlags;
                v = *t;
                v &= m20;
                flags = t;
                if (TestFlagsValue(v)) {
                    TerrainModifiers_UnregisterCell(&actor->terrainCell);
                    {
                        u32 mask = ~0x20;
                        *flags &= mask;
                    }
                }
            }
            Heap_Free(actor);
        }
    }
    gRegionActorPool = 0;
    return 0;
}

s32 Pool128_InitManager(struct RegionSpriteManager *manager)
{
    manager->sequenceResource = ScriptProc_Start(0x922e, 0x1003);
    manager->alternateResource = ScriptProc_Start(0x922e, 0x931e);
    gRegionActorPool = manager;
    manager->used = 0;
    manager->cursor = 0xff;
    return 0;
}

struct RegionSpriteManager *Pool1B4_Create(void)
{
    struct RegionSpriteManager *manager;
    manager = Task_Create(10, sizeof(*manager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void (*)(void *))RegionActorPool_UpdateActors,
                          (void (*)(void *))RegionActorPool_DestroyActors);
        if (Pool128_InitManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

void Pool1B4_ResetInstance(void)
{
    gRegionActorPool = 0;
}
