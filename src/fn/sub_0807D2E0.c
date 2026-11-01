#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.3%
#include "scene_actor_part_burst.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain_reaction.h"

#include "scene_actor_reset.h"
#include "engine.h"

#include "scene_actor_damage_visibility.h"
#include "runtime.h"
#include "ram.h"

void SceneActorRecord_RecordPair(u32, u32);
void SceneActorAttachment_ReplaceResponseSlot(struct SceneActorRegistryResetView *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(u8 *, s32);

static inline void ClearDamageOffset(vu32 *zero, void *dst)
{
    *zero = 0;
    CpuSet((const void *)zero, dst, 0x05000002);
}

static inline void ShowDamage(struct SceneActorRegistryResetView *a, vu32 *zero)
{
    u32 buffer[2];
    s32 health = a->health;
    u32 max = (u16)a->defaultHealth;
    s32 kind = (s16)a->fieldB2;
    s32 param = (s16)a->field9A;
    SceneHud_SetTimedState(a->body.id, health, max, kind, 0, param);
    ClearDamageOffset(zero, buffer);
    if (a->reactionSoundVariant || !(a->field2D8 & 0x20000))
        DamageNumberDisplay_Show(&a->colliderA, a->reactionSoundVariant, 0, &a->body.position,
                                 buffer, 1);
}

static inline u16 *MotionFlags(struct SceneActorRegistryResetView *a)
{
    return &a->flags2D4;
}
static inline struct ContactCollider *PrimaryCollider(struct SceneActorRegistryResetView *a)
{
    return &a->colliderA;
}
struct HitMotionFlags {
    s16 bits : 16;
};
static inline void MarkHitFlag(s16 bit, struct HitMotionFlags *flags)
{
    flags->bits = bit | flags->bits;
}

static inline u32 *StatusFlags(struct SceneActorRegistryResetView *actor)
{
    return &actor->flags2D0;
}
static inline u32 ReadStatus(struct SceneActorRegistryResetView *actor, u32 bits)
{
    return actor->flags2D0 & bits;
}
static inline u8 GetResponseKind(struct SceneActorRegistryResetView *a)
{
    return a->field2B5;
}
static inline void SelectAnimationState(struct SceneActorRegistryResetView *a, u8 state, u8 phase,
                                        u8 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
}
static inline void ResetEntry(struct SceneActorRegistryResetView *a, u32 ready)
{
    a->counter2AC = 0;
    a->field2B1 = ready;
}
static inline void MaskContactFlags(u32 *dst, u32 mask)
{
    *dst = mask & *dst;
}
static inline u32 ReadControlFlag(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2CC & mask;
}
static inline struct SceneActorRegistryControl *GetControl(struct SceneActorRegistryResetView *a)
{
    return &a->group.common.control;
}
union HitSparkTable {
    const u16 *table;
    struct {
        u16 offset;
        u8 page;
        u8 region;
    } encoded;
};
static inline const u16 *ReadSparkTable(const union HitSparkTable *source)
{
    return source->table;
}

static inline s32 HitResponse_HalfHealth(s16 health)
{
    if (health >= 0)
        return health >> 1;
    return -((-health) >> 1);
}

void SceneActorReaction_ApplyHitResponse(void *victim, u32 mode, struct ContactPosition *position,
                                         u32 heightOffset, u32 fullDamage, u32 unused)
{
    struct SceneActorRegistryResetView *a = victim;
    struct SceneActorChaseGroup *group = &a->group;
    struct ContactCollider *collider = &a->colliderA;
    s32 category;
    vu32 zero;

    u32 flag;
    u32 *status;
    struct SceneActorRegistryControl *control;
    category = a->category;
    a->field2B8 = 0;

    flag = 0x2000;
    if (a->flags2D4 & flag)
        SceneActorEffects_SpawnContactParts((void *)a);
    {
        struct HitMotionFlags *field = (void *)MotionFlags(a);
        s32 bits = ~0x2000;
        field->bits &= bits;
    }
    switch ((s32)mode) {
        case 0:
        case 1:
            status = &a->flags2D0;
            if (GetResponseKind(a) != 0x15) {
                SceneActorTerrain_EnterState(a, 3, 0, 0);
                SceneActorTerrain_InstallControl(a, group->field5FC, 0x15);
                a->field2B8 = 0;
                {
                    u32 mask = 0x20000;
                    *status = mask | *status;
                }
            }
            {
                u32 mask = 4;
                *status = mask | *status;
            }
            MaskContactFlags(&a->flags2CC, ~0x400000);
            ((struct SceneActorRegistryResetView *)victim)->field2C2 = 0x40;
            ((struct SceneActorRegistryResetView *)victim)->group.common.gate.flags = 0;
            ((struct SceneActorRegistryResetView *)victim)->group.common.gate.counter = 0;
            {
                u32 mask = ~0x380;
                ((struct SceneActorRegistryResetView *)victim)->flags2D4 =
                    mask & ((struct SceneActorRegistryResetView *)victim)->flags2D4;
            }
            ((struct SceneActorRegistryResetView *)victim)->tracking.flags &= ~0x18;

            if (ReadControlFlag(victim, 0x200000)) {
                *StatusFlags(victim) &= ~0x1000000;
                {
                    u32 mask = ~0x200;
                    ((struct SceneActorRegistryResetView *)victim)->flags2D4 =
                        mask & ((struct SceneActorRegistryResetView *)victim)->flags2D4;
                }
                control = GetControl(victim);
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
            {
                u32 mask = ~0x400;
                ((struct SceneActorRegistryResetView *)victim)->flags2D4 =
                    mask & ((struct SceneActorRegistryResetView *)victim)->flags2D4;
            }
            ((struct SceneActorRegistryResetView *)victim)->group.turn.turnsRemaining = 0xFF;
            a->body.position = *position;
            a->body.position.y += heightOffset;
            if (a->drawableKind == 0)
                *(struct ContactPosition *)((u8 *)a->drawable + 0x1C) = a->body.position;
            else
                *(struct ContactPosition *)((u8 *)a->drawable + 0x40) = a->body.position;
            {
                u32 mask = 0x2000;
                a->flags2D6 = mask | a->flags2D6;
            }
            break;
        case 2: {
            u32 mask = 4;
            *StatusFlags(a) |= mask;
            ActorShadow_Disable(a->shadow);
            {
                u32 mask = 1;
                if (a->drawableKind == 0)
                    *(u32 *)a->drawable |= mask;
                else {
                    struct SceneActorPlacementTileFlags *draw = (void *)((u8 *)a->drawable + 0x20);
                    draw->flags |= mask;
                }
            }
            if ((s32)fullDamage > 0) {
                a->field2EA = 0;
                {
                    s32 damage = a->health;
                    collider->cost = damage;
                    a->reactionSoundVariant = damage;
                }
                a->health = 0;
                MarkHitFlag(8, (void *)&a->flags2D4);
                {
                    u32 mask = 0x400;
                    a->flags2CC = mask | a->flags2CC;
                }
                {
                    u32 mask = 0xA0000;
                    *StatusFlags(a) |= mask;
                }
                SceneActorAttachment_ReplaceResponseSlot(victim, 0, 0x7F, 0);
                SceneActorAttachment_ReplaceResponseSlot(victim, 1, 0x7F, 0);
                SceneActorAttachment_ReplaceResponseSlot(victim, 3, 0x7F, 0);
                SceneActorEffects_AdvanceSteps(victim, 4);
                SceneActorTerrain_InstallControl(a, a->deathControl, 0x11);
                SelectAnimationState(a, 3, 0, 0);
                ResetEntry(a, 1);
                if (SceneActorDamage_ShouldShow(a))
                    ShowDamage(a, &zero);
                if (!SceneActorDamage_TestFlags(gEngineFlags, 0x4800) && !ReadStatus(a, 0x800))
                    SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)a->body.id);
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
                    if (a->category == 4 || a->category == 1 || a->category == 5 ||
                        a->category == 7 || a->category == 6 || a->category == 24 ||
                        a->category == 25)
                        Sound_PlaySong(0xE5);
                }
            } else {
                s32 half, hp;
                s32 left;

                half = HitResponse_HalfHealth(a->health);
                collider->cost = half;
                a->reactionSoundVariant = half;
                hp = a->health;
                left = (s16)(hp - PrimaryCollider(a)->cost);
                if (left <= 0)
                    left = 0;
                a->health = left;
                if (SceneActorDamage_ShouldShow(a))
                    ShowDamage(a, &zero);
            }
            break;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_ApplyHitResponse);
#endif
