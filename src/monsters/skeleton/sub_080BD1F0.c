#include "engine/shared/scene_actor_chase_shared.h"
#include "actors/scene_actor_behavior/scene_actor_behavior.h"
#include "monsters/skeleton/skeleton.h"
#include "actors/scene_actor_reaction/scene_actor_reaction.h"
#include "engine/script_utility_references/script_utility.h"
#include "engine/shared/engine.h"
#include "engine/shared/runtime.h"
#include "engine/shared/spark_particle.h"
#include "monsters/skeleton/enemy_defeat_statistics.h"

/* Skeleton defeat state (tentative). On entry: releases attachments, clears the route and,
 * for categories 6/7, emits a bone burst through sub_08127590. Then three outcomes:
 * flag 0x4000 (no reward) sparks and idles; while the context is not finished it switches to
 * SceneActorReaction_TickHiddenTimeout (revival is disabled for delays above 19999); otherwise the
 * full defeat: script, reward, kill counters per category (saturating at 9999, plus one
 * counter per collider reaction flag) and a six-spark burst.
 * The burst arguments live in function-scope variables and are rematerialized at each call. */
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void sub_08127590(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void sub_08159164(u32);
void Player_RefreshLifeState(u32, u32);
void Skeleton_InitializeCategoryProfile(void *);
void Skeleton_InitVariantMotion(void *);
void SceneActorRecord_RecordPair(u32, u32);


static inline void Count(s16 *p)
{
    if (*p < 9999)
        *p = *p + 1;
    else
        *p = 9999;
}
static inline s32 Clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}
static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline u32 Masked16(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    return *p & mask;
}
static inline u8 TestBits(u32 *p, u32 maskInput)
{
    u32 mask = maskInput;
    if (*p & mask)
        return 1;
    return 0;
}
static inline u32 Skeleton_StatusFlags(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.statusFlags & mask;
}
static inline u32 RandomFrom(const u16 *table, u32 wrap, u32 bits)
{
    u32 index = (gRandomIndex + 1) & wrap;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)(index + (u32)table);
    v &= bits;
    return v + 1;
}
static inline void Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
#define BURST(kind)                                                                                \
    do {                                                                                           \
        struct ContactPosition vec;                                                                \
        struct ContactPosition pos;                                                                \
        u32 angle = (actor->base.body.direction + 0x80) & 0xFF;                                    \
        Polar(&vec, angle, 10);                                                                    \
        pos = actor->base.body.position;                                                           \
        pos.y += 200;                                                                              \
        vec.y = 54;                                                                                \
        sub_08127590(&pos, &vec, 0xD45E, 0x1DC3, kind, 0x1A, 2, 0, 0x13C, 0, 0, 0, 0, 0, 0);       \
    } while (0)
#define FLAG_COUNTS()                                                                              \
    {                                                                                              \
        if (flags & 0x80)                                                                          \
            ((struct EnemyDefeatStatistics *)gGameState)->reactionCounts[0] =                      \
                Clamp(((struct EnemyDefeatStatistics *)gGameState)->reactionCounts[0] + 1);        \
        else if (flags & 0x100)                                                                    \
            ((struct EnemyDefeatStatistics *)gGameState)->reactionCounts[1] =                      \
                Clamp(((struct EnemyDefeatStatistics *)gGameState)->reactionCounts[1] + 1);        \
        else if (flags & 0x200)                                                                    \
            ((struct EnemyDefeatStatistics *)gGameState)->reactionCounts[2] =                      \
                Clamp(((struct EnemyDefeatStatistics *)gGameState)->reactionCounts[2] + 1);        \
        else if (flags & 0x400)                                                                    \
            ((struct EnemyDefeatStatistics *)gGameState)->reactionCounts[3] =                      \
                Clamp(((struct EnemyDefeatStatistics *)gGameState)->reactionCounts[3] + 1);        \
        else if (flags &= 0x800)                                                                   \
            ((struct EnemyDefeatStatistics *)gGameState)->reactionCounts[4] =                      \
                Clamp(((struct EnemyDefeatStatistics *)gGameState)->reactionCounts[4] + 1);        \
    }
static inline void RecordKill(struct ChaseActor *actor, u8 *category)
{
    struct ContactCollider *collider = &actor->base.colliderA;
    switch (*category) {
        case 4: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category4);
            FLAG_COUNTS();
            break;
        }
        case 1: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category1);
            FLAG_COUNTS();
            break;
        }
        case 2: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category2);
            FLAG_COUNTS();
            break;
        }
        case 3: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category3);
            FLAG_COUNTS();
            break;
        }
        case 14: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category14);
            FLAG_COUNTS();
            break;
        }
        case 24: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category24);
            FLAG_COUNTS();
            break;
        }
        case 10: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category10);
            FLAG_COUNTS();
            break;
        }
        case 5:
        case 6:
        case 7: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category5To7);
            FLAG_COUNTS();
            break;
        }
        case 21: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category21);
            FLAG_COUNTS();
            break;
        }
        case 18: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category18);
            FLAG_COUNTS();
            break;
        }
        case 19: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category19);
            FLAG_COUNTS();
            break;
        }
        case 17: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category17);
            FLAG_COUNTS();
            break;
        }
        case 22: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category22);
            FLAG_COUNTS();
            break;
        }
        case 20: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category20);
            FLAG_COUNTS();
            break;
        }
        case 9: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category9);
            FLAG_COUNTS();
            break;
        }
        case 23: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category23);
            FLAG_COUNTS();
            break;
        }
        case 12:
        case 13: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category12And13);
            FLAG_COUNTS();
            break;
        }
        case 25: {
            u32 flags = collider->reactionFlags;
            Count(&((struct EnemyDefeatStatistics *)gGameState)->category25);
            FLAG_COUNTS();
            break;
        }
    }
}

void Skeleton_UpdateDefeat(struct ChaseActor *actor)
{
    s32 values[8];
    /* Allocation-only: reuse disjoint pointer and spark-priority lifetimes. */
    union {
        struct Skeleton *context;
        s32 one;
    } state = {actor->base.context};
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    u32 mask;
    u32 wrap = 1023;
    u32 bits = 3;
    s32 count = 4;
    s32 life = 20;
    s32 size = 2;
    union {
        u16 *flags;
        s32 one;
    } response;
    u32 *ctxFlags;
    u8 blocked;
    u8 *categoryp;
    const u16 *lateTable;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->base.restart = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        SceneActorPlacement_PlayCategoryEventSound(actor);
        {
            struct SceneActorRegistryControl *control = &group->control;
            u32 *field = &actor->base.behaviorFlags;
            u32 mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            control->field14 = 0;
            control->timer = 0;
        }
        actor->base.colliderA.flags |= 4;
        if (actor->base.category == 6)
            BURST(0);
        else if (actor->base.category == 7)
            BURST(4);
        {
            u32 m = 0x1800;
            actor->base.placementFlags = m | actor->base.placementFlags;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    /* Allocation-only: the two bounded exit arms preserve this global table
     * lifetime until jump2; neither arm nor this initial copy emits code. */
    lateTable = (const u16 *)actor;
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    actor->base.attachmentTimer = 0;
    actor->base.colliderA.responseValue = 0;
    mask = 0x4000;
    {
        u32 test = actor->base.motionFlags & mask;
        response.flags = &actor->base.motionFlags;
        if (!test)
            goto notHidden;
    }
    {
        u16 script = (u16)actor->base.script1F8;
        if (script) {
            struct ScriptUtilityArguments arguments;
            values[0] = actor->base.body.position.x;
            values[1] = actor->base.body.position.y;
            values[2] = actor->base.body.position.z;
            values[3] = actor->base.parameters1FC[0];
            values[4] = actor->base.parameters1FC[1];
            values[5] = actor->base.parameters1FC[2];
            values[6] = actor->base.hitOwners;
            values[7] = actor->base.body.id;
            arguments.count = 8;
            arguments.values = values;
            Script_Call(script, &arguments);
        }
        {
            u32 t = *response.flags & mask;
            categoryp = &actor->base.category;
            if (!t && (u32)(*categoryp - 11) > 2)
                Bestiary_UnlockEntry(
                    Bestiary_GetEntryForActor(*categoryp, actor->base.paletteOffset));
        }
        {
            struct ContactPosition range;
            const u16 *table;
            u8 category;
            range.x = 80;
            range.y = 60;
            range.z = 60;
            table = gRandomTable;
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(table, wrap, bits), &range,
                                0x20, count, life, size, state.one = 1);
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(table, wrap, bits), &range,
                                0x40, count, life, size, state.one);
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(table, wrap, bits), &range,
                                0x5E, count, life, size, state.one);
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(table, wrap, bits), &range,
                                0x80, count, life, size, state.one);
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(table, wrap, bits), &range,
                                0xC4, count, life, size, state.one);
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(table, wrap, bits), &range,
                                0xF0, count, life, size, state.one);
            category = *categoryp;
            if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
                category == 0x18 || category == 0x19)
                /* The late zero retains the response/table conflict through combine. */
                Sound_PlaySong(0xE5 | (((u32)response.flags & 31) >> 5));
        }

        {
            u32 bound = (u8)(u32)actor;
            if ((s32)bound < 0)
                Sound_PlaySong((u32)lateTable);
        }
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        SceneActorChase_SetState(actor, 3, 0, 0);
        {
            u32 m = 0x1000;
            actor->base.reactionFlags = m | actor->base.reactionFlags;
        }
        ActorShadow_Disable(actor->base.shadow);
        Trigger_ConsumePending(actor->base.receiver, 1);
        return;
    }
notHidden:
    if (actor->base.hitReactionFlags & 0x7F) {
        u32 hit = actor->base.hitAttributes & 0x100017;
        ctxFlags = &state.context->actionFlags;
        if (hit)
            goto defeated;
    }
    {
        u32 m = 0x1800;
        u32 *f = &state.context->actionFlags;
        u32 v = *f & m;
        ctxFlags = f;
        blocked = SceneActorChase_Test(v, ~0);
    }
    if (!blocked) {
        SceneActorChase_SetControl(actor, SceneActorReaction_TickHiddenTimeout, 0x39);
        SceneActorChase_SetState(actor, 3, 0, 0);
        actor->base.health = 1;
        {
            u32 bit = 0x40;
            *response.flags &= ~bit;
            if (SceneActorChase_Test(*ctxFlags, bit)) {
                u32 c = 7;
                actor->base.category = c;
                Skeleton_InitializeCategoryProfile(actor);
                Skeleton_InitVariantMotion(actor);
            }
        }
        if (state.context->revivalDelay > 19999) {
            u16 script = (u16)actor->base.script1F8;
            if (script) {
                struct ScriptUtilityArguments arguments;
                values[0] = actor->base.body.position.x;
                values[1] = actor->base.body.position.y;
                values[2] = actor->base.body.position.z;
                values[3] = actor->base.parameters1FC[0];
                values[4] = actor->base.parameters1FC[1];
                values[5] = actor->base.parameters1FC[2];
                values[6] = actor->base.hitOwners;
                values[7] = actor->base.body.id;
                arguments.count = 8;
                arguments.values = values;
                Script_Call(script, &arguments);
            }
            if (!Masked16(response.flags, 0x4000) && (u32)(actor->base.category - 11) > 2)
                Bestiary_UnlockEntry(
                    Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
            sub_08159164(state.context->defeatExperience);
            Player_RefreshLifeState(actor->base.hitAttributes, actor->base.fieldA1);
        } else {
            sub_08159164(state.context->downedExperience);
        }
        {
            u32 m = 0x10;
            *ctxFlags |= m;
        }
        Sound_PlaySong(0x3AD);
        {
            u32 bound = ((u32)actor & 4095);
            if ((s32)bound < 0)
                Sound_PlaySong((u32)lateTable);
        }

    } else {
    defeated:
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        SceneActorChase_SetState(actor, 1, 0, 2);
        {
            u8 b = TestBits(ctxFlags, 0x40);
            categoryp = &actor->base.category;
            if (b)
                *categoryp = 7;
        }
        SceneActorBehavior_CallTerrainScript(actor);
        {
            u16 script = (u16)actor->base.script1F8;
            if (script) {
                struct ScriptUtilityArguments arguments;
                values[0] = actor->base.body.position.x;
                values[1] = actor->base.body.position.y;
                values[2] = actor->base.body.position.z;
                values[3] = actor->base.parameters1FC[0];
                values[4] = actor->base.parameters1FC[1];
                values[5] = actor->base.parameters1FC[2];
                values[6] = actor->base.hitOwners;
                values[7] = actor->base.body.id;
                arguments.count = 8;
                arguments.values = values;
                Script_Call(script, &arguments);
            }
        }
        if (!Masked16(response.flags, 0x4000) && (u32)(*categoryp - 11) > 2)
            Bestiary_UnlockEntry(Bestiary_GetEntryForActor(*categoryp, actor->base.paletteOffset));
        sub_08159164(state.context->defeatExperience);
        Player_RefreshLifeState(actor->base.hitAttributes, actor->base.fieldA1);
        Count(&((struct EnemyDefeatStatistics *)gGameState)->lifetimeTotal);
        Count(&((struct EnemyDefeatStatistics *)gGameState)->total);
        RecordKill(actor, categoryp);
        {
            struct ContactPosition range;
            u8 category;
            range.x = 80;
            range.y = 60;
            range.z = 60;
            lateTable = gRandomTable;
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(lateTable, wrap, bits),
                                &range, 0x20, count, life, size, response.one = 1);
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(lateTable, wrap, bits),
                                &range, 0x40, count, life, size, response.one);
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(lateTable, wrap, bits),
                                &range, 0x5E, count, life, size, response.one);
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(lateTable, wrap, bits),
                                &range, 0x80, count, life, size, response.one);
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(lateTable, wrap, bits),
                                &range, 0xC4, count, life, size, response.one);
            SparkParticle_Spawn(&actor->base.body.position, RandomFrom(lateTable, wrap, bits),
                                &range, 0xF0, count, life, size, response.one);
            category = *categoryp;
            if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
                category == 0x18 || category == 0x19)
                Sound_PlaySong(0xE5);
        }
        if (!SceneActorChase_Test(gEngineFlags, 0x4800) && !Skeleton_StatusFlags(actor, 0x800))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->base.body.id);
    }
    ActorShadow_Disable(actor->base.shadow);
}
