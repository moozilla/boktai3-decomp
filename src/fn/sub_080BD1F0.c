// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
#include "runtime.h"

/* Skeleton defeat state (tentative). On entry: releases attachments, clears the route and,
 * for categories 6/7, emits a bone burst through sub_08127590. Then three outcomes:
 * flag 0x4000 (no reward) sparks and idles; while the context is not finished it switches to
 * SceneActorReaction_TickHiddenTimeout (revival, with the rank 20000 reward); otherwise the
 * full defeat: script, reward, kill counters per category (saturating at 9999, plus one
 * counter per collider reaction flag) and a six-spark burst.
 * Pins: the spark table and the constant 1 of the two spark bursts. Kill counters are an
 * if/else, as in Ivy_UpdateDefeat, which keeps them pin-free. The burst arguments
 * live in function-scope variables, which the ROM rematerialises at each call. */
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
void sub_08127590(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void sub_08159164(u32);
void Player_RefreshLifeState(u32, u32);
void Skeleton_InitializeCategoryProfile(void *);
void Skeleton_InitVariantMotion(void *);
void SceneActorReaction_TickHiddenTimeout(void);
void SceneActorRecord_RecordPair(u32, u32);

struct SkeletonContext {
    u8 unknown0[0x6D2];
    u16 rank;
    u16 rewardLow;
    u16 rewardHigh;
    u8 unknown6D8[4];
    u32 flags;
};

#define STAT(n)  (*(s16 *)((u8 *)gGameState + (n)))
#define COUNT(n) Count(&STAT(n))
#define INC(n)   STAT(n) = Clamp(STAT(n) + 1)

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
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
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
static inline u32 Flag(struct SceneActorRegistryResetView *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}
static inline void SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    u32 state = s;
    actor->state2A8 = state;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
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
        u32 angle = (actor->direction + 0x80) & 0xFF;                                              \
        Polar(&vec, angle, 10);                                                                    \
        pos = actor->position;                                                                     \
        pos.y += 200;                                                                              \
        vec.y = 54;                                                                                \
        sub_08127590(&pos, &vec, 0xD45E, 0x1DC3, kind, 0x1A, 2, 0, 0x13C, 0, 0, 0, 0, 0, 0);       \
    } while (0)
#define FLAG_COUNTS()                                                                              \
    {                                                                                              \
        if (flags & 0x80)                                                                          \
            INC(0x58A);                                                                            \
        else if (flags & 0x100)                                                                    \
            INC(0x58C);                                                                            \
        else if (flags & 0x200)                                                                    \
            INC(0x58E);                                                                            \
        else if (flags & 0x400)                                                                    \
            INC(0x590);                                                                            \
        else if (flags &= 0x800)                                                                   \
            INC(0x592);                                                                            \
    }
static inline void RecordKill(struct SceneActorRegistryResetView *actor, u8 *category)
{
    struct ContactCollider *collider = &actor->colliderA;
    switch (*category) {
        case 4: {
            u32 flags = collider->reactionFlags;
            COUNT(0x53A);
            FLAG_COUNTS();
            break;
        }
        case 1: {
            u32 flags = collider->reactionFlags;
            COUNT(0x53C);
            FLAG_COUNTS();
            break;
        }
        case 2: {
            u32 flags = collider->reactionFlags;
            COUNT(0x53E);
            FLAG_COUNTS();
            break;
        }
        case 3: {
            u32 flags = collider->reactionFlags;
            COUNT(0x540);
            FLAG_COUNTS();
            break;
        }
        case 14: {
            u32 flags = collider->reactionFlags;
            COUNT(0x54C);
            FLAG_COUNTS();
            break;
        }
        case 24: {
            u32 flags = collider->reactionFlags;
            COUNT(0x55C);
            FLAG_COUNTS();
            break;
        }
        case 10: {
            u32 flags = collider->reactionFlags;
            COUNT(0x546);
            FLAG_COUNTS();
            break;
        }
        case 5:
        case 6:
        case 7: {
            u32 flags = collider->reactionFlags;
            COUNT(0x542);
            FLAG_COUNTS();
            break;
        }
        case 21: {
            u32 flags = collider->reactionFlags;
            COUNT(0x554);
            FLAG_COUNTS();
            break;
        }
        case 18: {
            u32 flags = collider->reactionFlags;
            COUNT(0x550);
            FLAG_COUNTS();
            break;
        }
        case 19: {
            u32 flags = collider->reactionFlags;
            COUNT(0x552);
            FLAG_COUNTS();
            break;
        }
        case 17: {
            u32 flags = collider->reactionFlags;
            COUNT(0x54E);
            FLAG_COUNTS();
            break;
        }
        case 22: {
            u32 flags = collider->reactionFlags;
            COUNT(0x556);
            FLAG_COUNTS();
            break;
        }
        case 20: {
            u32 flags = collider->reactionFlags;
            COUNT(0x558);
            FLAG_COUNTS();
            break;
        }
        case 9: {
            u32 flags = collider->reactionFlags;
            COUNT(0x544);
            FLAG_COUNTS();
            break;
        }
        case 23: {
            u32 flags = collider->reactionFlags;
            COUNT(0x55A);
            FLAG_COUNTS();
            break;
        }
        case 12:
        case 13: {
            u32 flags = collider->reactionFlags;
            COUNT(0x54A);
            FLAG_COUNTS();
            break;
        }
        case 25: {
            u32 flags = collider->reactionFlags;
            COUNT(0x560);
            FLAG_COUNTS();
            break;
        }
    }
}

void Skeleton_UpdateDefeat(struct SceneActorRegistryResetView *actor)
{
    s32 values[8];
    struct SkeletonContext *context = actor->context;
    u8 *group = (u8 *)&actor->group;
    u32 mask;
    u32 wrap = 1023;
    u32 bits = 3;
    s32 count = 4;
    s32 life = 20;
    s32 size = 2;
    u16 *flags2D4;
    u32 *ctxFlags;
    u8 blocked;
    u8 *categoryp;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->field2B3 = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        SceneActorPlacement_PlayCategoryEventSound(actor);
        {
            struct SceneActorRegistryControl *control = (void *)(group + 0x4C);
            u32 *field = &actor->flags2CC;
            u32 mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            *(u16 *)((u8 *)control + 0x14) = 0;
            control->timer = 0;
        }
        actor->colliderA.flags |= 4;
        if (actor->category == 6)
            BURST(0);
        else if (actor->category == 7)
            BURST(4);
        {
            u32 m = 0x1800;
            actor->flags2C0 = m | actor->flags2C0;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    actor->fieldAA = 0;
    actor->colliderA.responseValue = 0;
    mask = 0x4000;
    {
        u32 test = actor->flags2D4 & mask;
        flags2D4 = &actor->flags2D4;
        if (!test)
            goto notHidden;
    }
    {
        u16 script = *(u16 *)((u8 *)actor + 0x1F8);
        if (script) {
            struct ScriptUtilityArguments arguments;
            values[0] = actor->position.x;
            values[1] = actor->position.y;
            values[2] = actor->position.z;
            values[3] = *(s32 *)((u8 *)actor + 0x1FC);
            values[4] = *(s32 *)((u8 *)actor + 0x200);
            values[5] = *(s32 *)((u8 *)actor + 0x204);
            values[6] = actor->field2E0;
            values[7] = actor->id;
            arguments.count = 8;
            arguments.values = values;
            Script_Call(script, &arguments);
        }
        {
            u32 t = *flags2D4 & mask;
            categoryp = &actor->category;
            if (!t && (u32)(*categoryp - 11) > 2)
                GameState_SetEventFlag(
                    BestiaryDisplay_GetEntryForActor(*categoryp, actor->paletteOffset));
        }
        {
            struct ContactPackedVector range;
            const u16 *table;
            register s32 one asm("sb");
            u8 category;
            range.x = 80;
            range.y = 60;
            range.z = 60;
            table = gUnk_0203B400;
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x20,
                                count, life, size, one = 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x40,
                                count, life, size, one);
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x5E,
                                count, life, size, one);
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x80,
                                count, life, size, one);
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0xC4,
                                count, life, size, one);
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0xF0,
                                count, life, size, one);
            category = *categoryp;
            if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
                category == 0x18 || category == 0x19)
                Sound_PlaySong(0xE5);
        }
        SetCtl(actor, (void *)SceneActorControl_OnIdle, 16);
        SetState(actor, 3, 0, 0);
        {
            u32 m = 0x1000;
            actor->flags2D6 = m | actor->flags2D6;
        }
        ActorShadow_Disable((u8 *)actor + 0x320);
        Trigger_ConsumePending((u8 *)actor + 0xC, 1);
        return;
    }
notHidden:
    if (actor->field2DC & 0x7F) {
        u32 hit = actor->field2D8 & 0x100017;
        ctxFlags = &context->flags;
        if (hit)
            goto defeated;
    }
    {
        u32 m = 0x1800;
        u32 *f = &context->flags;
        u32 v = *f & m;
        ctxFlags = f;
        blocked = Test(v, ~0);
    }
    if (!blocked) {
        {
            void *callback = (void *)SceneActorReaction_TickHiddenTimeout;
            u32 kind = 0x39;
            actor->field2B2 = 1;
            actor->field2B0 = blocked;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = blocked;
            {
                u32 *field = &actor->flags114;
                s32 clear = ~1;
                *field &= clear;
            }
            actor->word104 = blocked;
        }
        {
            u32 state = 3;
            actor->state2A8 = state;
        }
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = blocked;
        actor->field2B1 = 1;
        actor->health = 1;
        {
            u32 bit = 0x40;
            *flags2D4 &= ~bit;
            if (Test(*ctxFlags, bit)) {
                u32 c = 7;
                actor->category = c;
                Skeleton_InitializeCategoryProfile(actor);
                Skeleton_InitVariantMotion(actor);
            }
        }
        if (context->rank > 19999) {
            u16 script = *(u16 *)((u8 *)actor + 0x1F8);
            if (script) {
                struct ScriptUtilityArguments arguments;
                values[0] = actor->position.x;
                values[1] = actor->position.y;
                values[2] = actor->position.z;
                values[3] = *(s32 *)((u8 *)actor + 0x1FC);
                values[4] = *(s32 *)((u8 *)actor + 0x200);
                values[5] = *(s32 *)((u8 *)actor + 0x204);
                values[6] = actor->field2E0;
                values[7] = actor->id;
                arguments.count = 8;
                arguments.values = values;
                Script_Call(script, &arguments);
            }
            if (!Masked16(flags2D4, 0x4000) && (u32)(actor->category - 11) > 2)
                GameState_SetEventFlag(
                    BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
            sub_08159164(context->rewardHigh);
            Player_RefreshLifeState(actor->field2D8, *((u8 *)actor + 0xA1));
        } else {
            sub_08159164(context->rewardLow);
        }
        {
            u32 m = 0x10;
            *ctxFlags |= m;
        }
        Sound_PlaySong(0x3AD);
    } else {
    defeated:
        SetCtl(actor, (void *)SceneActorControl_OnIdle, 16);
        SetState(actor, 1, 0, 2);
        {
            u8 b = TestBits(ctxFlags, 0x40);
            categoryp = &actor->category;
            if (b)
                *categoryp = 7;
        }
        SceneActorBehavior_CallTerrainScript(actor);
        {
            u16 script = *(u16 *)((u8 *)actor + 0x1F8);
            if (script) {
                struct ScriptUtilityArguments arguments;
                values[0] = actor->position.x;
                values[1] = actor->position.y;
                values[2] = actor->position.z;
                values[3] = *(s32 *)((u8 *)actor + 0x1FC);
                values[4] = *(s32 *)((u8 *)actor + 0x200);
                values[5] = *(s32 *)((u8 *)actor + 0x204);
                values[6] = actor->field2E0;
                values[7] = actor->id;
                arguments.count = 8;
                arguments.values = values;
                Script_Call(script, &arguments);
            }
        }
        if (!Masked16(flags2D4, 0x4000) && (u32)(*categoryp - 11) > 2)
            GameState_SetEventFlag(
                BestiaryDisplay_GetEntryForActor(*categoryp, actor->paletteOffset));
        sub_08159164(context->rewardHigh);
        Player_RefreshLifeState(actor->field2D8, *((u8 *)actor + 0xA1));
        COUNT(0x87C);
        COUNT(0x538);
        RecordKill(actor, categoryp);
        {
            struct ContactPackedVector range;
            register const u16 *table asm("sl");
            register s32 one asm("r8");
            u8 category;
            range.x = 80;
            range.y = 60;
            range.z = 60;
            table = gUnk_0203B400;
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x20,
                                count, life, size, one = 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x40,
                                count, life, size, one);
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x5E,
                                count, life, size, one);
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x80,
                                count, life, size, one);
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0xC4,
                                count, life, size, one);
            SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0xF0,
                                count, life, size, one);
            category = *categoryp;
            if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
                category == 0x18 || category == 0x19)
                Sound_PlaySong(0xE5);
        }
        if (!Test(gEngineFlags, 0x4800) && !Flag(actor, 0x800, 0x2D0))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->id);
    }
    ActorShadow_Disable((u8 *)actor + 0x320);
}
