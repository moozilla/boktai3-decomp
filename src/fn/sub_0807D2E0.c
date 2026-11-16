#include "scene_actor_part_burst.h"
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain_reaction.h"

#include "scene_actor_reset.h"
#include "engine.h"

#include "scene_actor_damage_visibility.h"
#include "runtime.h"
#include "ram.h"

void SceneActorRecord_RecordPair(u32, u32);
void SceneActorAttachment_ReplaceResponseSlot(struct ChaseActor *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(u8 *, s32);

static inline u8 HitResponse_Test(u32 value)
{
    u8 matches = 0;
    if (value)
        matches = 1;
    return matches;
}
static inline void ShowDamage(struct ChaseActor *a)
{
    u32 buffer[2];
    s32 health = a->base.health;
    u32 max = (u16)a->base.defaultHealth;
    s32 kind = (s16)a->base.hudKind;
    s32 param = (s16)a->base.hudLabel;
    SceneHud_SetTimedState(a->base.body.id, health, max, kind, 0, param);
    ZeroFill8(buffer);
    if (a->base.reactionSoundVariant || !(a->base.hitAttributes & 0x20000))
        DamageNumberDisplay_Show(&a->base.colliderA, a->base.reactionSoundVariant, 0,
                                 &a->base.body.position, buffer, 1);
}

static inline u16 *MotionFlags(struct ChaseActor *a)
{
    return &a->base.motionFlags;
}
static inline struct ContactCollider *PrimaryCollider(struct ChaseActor *a)
{
    return &a->base.colliderA;
}
struct HitMotionFlags {
    s16 bits : 16;
};
static inline void MarkHitFlag(s16 bit, struct HitMotionFlags *flags)
{
    flags->bits = bit | flags->bits;
}

static inline u32 *StatusFlags(struct ChaseActor *actor)
{
    return &actor->base.statusFlags;
}
static inline u32 ReadStatus(struct ChaseActor *actor, u32 bits)
{
    return actor->base.statusFlags & bits;
}
static inline void SelectAnimationState(struct ChaseActor *a, u8 state, u8 phase, u8 behavior)
{
    a->base.state2A8 = state;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
}
static inline void ResetEntry(struct ChaseActor *a, u32 ready)
{
    a->base.counter2AC = 0;
    a->base.field2B1 = ready;
}
static inline void MaskContactFlags(u32 *dst, u32 mask)
{
    *dst = mask & *dst;
}
static inline u32 ReadControlFlag(struct ChaseActor *a, u32 mask)
{
    return a->base.behaviorFlags & mask;
}
union HitSparkTable {
    const u16 *table;
    struct {
        u16 offset;
        u8 page;
        u8 region;
    } encoded;
};
static inline const u16 *ReadSparkTable(const union HitSparkTable *r)
{
    return r->table;
}

static inline s32 HitResponse_HalfHealth(s16 health)
{
    if (HitResponse_Test(health >= 0))
        return health >> 1;
    return -((-health) >> 1);
}

static inline struct SceneActorChaseTracking *ContactTracking(struct ChaseActor *actor)
{
    return &actor->base.tracking;
}

void SceneActorReaction_ApplyHitResponse(void *victim, u32 mode, struct ContactPosition *position,
                                         u32 heightOffset, u32 fullDamage, u32 unused)
{
    struct ChaseActor *a = victim;
    struct SceneActorChaseGroup *group = &a->group;
    struct ContactCollider *collider = &a->base.colliderA;
    s32 category;
    struct HitMotionFlags *motion;
    u32 partFlag;
    struct SceneActorRegistryControl *control;
    category = a->base.category;
    a->base.hitLockTimer = 0;

    partFlag = 0x2000;
    if (HitResponse_Test(a->base.motionFlags & partFlag))
        SceneActorEffects_SpawnContactParts((void *)a);
    {
        struct HitMotionFlags *field = (void *)MotionFlags(a);
        s32 bits = ~0x2000;
        field->bits &= bits;
        motion = field;
    }
    switch ((s32)mode) {
        case 0:
        case 1: {
            struct ChaseActor *state = victim;
            if (a->base.controlKind != 0x15) {
                SceneActorTerrain_EnterState(a, 3, 0, 0);
                SceneActorTerrain_InstallControl(a, group->fadeControl, 0x15);
                a->base.hitLockTimer = 0;
                {
                    u32 mask = 0x20000;
                    *StatusFlags(a) |= mask;
                }
            }
            {
                u32 mask = 4;
                *StatusFlags(a) |= mask;
            }
            MaskContactFlags(&a->base.behaviorFlags, ~0x400000);
            state->base.field2C2 = 0x40;
            state->group.common.gate.flags = 0;
            state->group.common.gate.counter = 0;
            {
                u16 *flags = &state->base.motionFlags;
                u32 mask = ~0x380;
                *flags = mask & *flags;
            }
            {
                struct SceneActorChaseTracking *tracking = ContactTracking(victim);
                u32 mask = ~0x18;
                tracking->flags = mask & tracking->flags;
            }

            control = &state->group.common.control;
            if (ReadControlFlag(victim, 0x200000)) {
                {
                    u32 *status = StatusFlags(victim);
                    u32 mask = ~0x1000000;
                    *status = mask & *status;
                }
                {
                    u32 mask = ~0x200;
                    state->base.motionFlags = mask & state->base.motionFlags;
                }

                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
            {
                u32 mask = ~0x400;
                state->base.motionFlags = mask & state->base.motionFlags;
            }
            state->group.turn.turnsRemaining = 0xFF;
            a->base.body.position = *position;
            a->base.body.position.y += heightOffset;
            if (a->base.drawableKind == 0)
                *(struct ContactPosition *)((u8 *)a->base.drawable + 0x1C) = a->base.body.position;
            else
                *(struct ContactPosition *)((u8 *)a->base.drawable + 0x40) = a->base.body.position;
            {
                u32 mask = 0x2000;
                a->base.reactionFlags = mask | a->base.reactionFlags;
            }
            break;
        }
        case 2: {
            {
                u32 mask = 4;
                *StatusFlags(a) |= mask;
            }
            ActorShadow_Disable(a->base.shadow);
            {
                u32 mask = 1;
                if (a->base.drawableKind == 0)
                    *(u32 *)a->base.drawable |= mask;
                else {
                    struct SceneActorPlacementTileFlags *draw =
                        (void *)((u8 *)a->base.drawable + 0x20);
                    draw->flags |= mask;
                }
            }
            if ((s32)fullDamage > 0) {
                a->base.heightOffset = 0;
                {
                    s32 damage = a->base.health;
                    collider->cost = damage;
                    a->base.reactionSoundVariant = damage;
                }
                a->base.health = 0;
                MarkHitFlag(8, motion);
                {
                    u32 mask = 0x400;
                    a->base.behaviorFlags = mask | a->base.behaviorFlags;
                }
                {
                    u32 mask = 0xA0000;
                    *StatusFlags(a) |= mask;
                }
                SceneActorAttachment_ReplaceResponseSlot(victim, 0, 0x7F, 0);
                SceneActorAttachment_ReplaceResponseSlot(victim, 1, 0x7F, 0);
                SceneActorAttachment_ReplaceResponseSlot(victim, 3, 0x7F, 0);
                SceneActorEffects_AdvanceSteps((u8 *)victim, 4);
                SceneActorTerrain_InstallControl(a, a->base.onDefeat, 0x11);
                SelectAnimationState(a, 3, 0, 0);
                ResetEntry(a, 1);
                if (SceneActorDamage_ShouldShow(a))
                    ShowDamage(a);
                if (!SceneActorDamage_TestFlags(gEngineFlags, 0x4800) && !ReadStatus(a, 0x800))
                    SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)a->base.body.id);
                if (category == 4 || category == 1) {
                    union SceneActorRegistryPackedPosition range;
                    union HitSparkTable source = {0};
                    u32 first;
                    const u16 *samples;
                    range.packed.x = 80;
                    range.packed.y = 60;
                    range.packed.z = 60;

                    source.encoded.offset = (u16)RANDOM_TABLE_ADDR;
                    source.encoded.page = (u8)(RANDOM_TABLE_ADDR >> 16);
                    source.encoded.region = (u8)(RANDOM_TABLE_ADDR >> 24);
                    samples = ReadSparkTable(&source);
                    first = SceneActorEffects_NextSparkCount(samples);
                    SceneActorEffects_SpawnDefeatSparks(a, &range.position, samples, first);
                    if (a->base.category == 4 || a->base.category == 1 || a->base.category == 5 ||
                        a->base.category == 7 || a->base.category == 6 || a->base.category == 24 ||
                        a->base.category == 25)
                        Sound_PlaySong(0xE5);
                }
            } else {
                s32 half, hp;
                s32 left;

                half = HitResponse_HalfHealth(a->base.health);
                collider->cost = half;
                a->base.reactionSoundVariant = half;
                hp = a->base.health;
                left = (s16)(hp - PrimaryCollider(a)->cost);
                if (left <= 0)
                    left = 0;
                a->base.health = left;
                if (SceneActorDamage_ShouldShow(a))
                    ShowDamage(a);
            }
            break;
        }
    }
}
