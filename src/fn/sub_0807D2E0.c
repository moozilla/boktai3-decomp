#include "scene_actor_part_burst.h"
#include "global.h"

#include "scene_actor_reset.h"
#include "engine.h"

#include "scene_actor_damage_visibility.h"
#include "runtime.h"
#include "ram.h"

void SceneActorRecord_RecordPair(u32, u32);
void SceneActorAttachment_ReplaceResponseSlot(u8 *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);

static inline void ClearDamageOffset(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}

static inline void ShowDamage(struct SceneActorRegistryResetView *a)
{
    u32 zero;
    u32 buffer[2];
    s32 health = a->health;
    u32 max = (u16)a->defaultHealth;
    s32 kind = (s16)a->fieldB2;
    s32 param = (s16)a->field9A;
    SceneHud_SetTimedState(a->body.id, health, max, kind, 0, param);
    ClearDamageOffset(&zero, buffer);
    if (a->reactionSoundVariant || !(a->field2D8 & 0x20000))
        DamageNumberDisplay_Show(&a->colliderA, a->reactionSoundVariant, 0, &a->body.position,
                                 buffer, 1);
}

static inline void SetReactionState(struct SceneActorRegistryResetView *a, u32 state, u32 phase,
                                    u32 behavior, u32 kind, void *target)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = target;
    a->field2C8 = 0;
    a->flags114 &= ~1;
    a->word104 = 0;
}

#ifdef NONMATCHING
// fuzzy: 54.9%
void SceneActorReaction_ApplyHitResponse(void *victim, u32 mode, struct ContactPosition *position, u32 p3, u32 flip,
                  u32 p5)
{
    struct SceneActorRegistryResetView *a = victim;
    struct SceneActorRegistryControlGroup *group = &a->group.common;
    s32 category;

    category = a->category;
    a->field2B8 = 0;

    if (a->flags2D4 & 0x2000)
        SceneActorEffects_SpawnContactParts((void *)a);
    a->flags2D4 &= ~0x2000;

    if ((s32)mode < 0)
        return;
    if (mode == 2)
        goto mode2;
    if (mode > 1)
        return;

    if (a->field2B5 != 0x15) {
        SetReactionState(a, 3, 0, 0, 0x15, *(void **)((u8 *)group + 0x228));
        a->field2B8 = 0;
        a->flags2D0 |= 0x20000;
    }
    a->flags2D0 |= 4;
    a->flags2CC &= ~0x400000;
    a->field2C2 = 0x40;
    group->gate.flags = 0;
    group->gate.counter = 0;
    a->flags2D4 &= ~0x380;
    a->tracking.flags &= ~0x18;
    if (a->flags2CC & 0x200000) {
        a->flags2D0 &= ~0x1000000;
        a->flags2D4 &= ~0x200;
        group->control.mode = 0;
        group->control.counter = 0;
        group->control.timer = 0;
    }
    a->flags2D4 &= ~0x400;
    a->group.turn.turnsRemaining = 0xFF;
    a->body.position = *position;
    a->body.position.y += p3;
    if (a->drawableKind == 0)
        *(struct ContactPosition *)((u8 *)a->drawable + 0x1C) = a->body.position;
    else
        *(struct ContactPosition *)((u8 *)a->drawable + 0x40) = a->body.position;
    a->flags2D6 |= 0x2000;
    return;

mode2:
    a->flags2D0 |= 4;
    ActorShadow_Disable((u8 *)a + 0x320);
    if (a->drawableKind == 0)
        *(u32 *)a->drawable |= 1;
    else
        *(u32 *)((u8 *)a->drawable + 0x28) |= 1;
    if ((s32)flip > 0) {
        a->field2EA = 0;
        a->reactionSoundVariant = a->health;
        *(s16 *)((u8 *)&a->colliderA + 0x3E) = a->health;
        a->health = 0;
        a->flags2D4 |= 8;
        a->flags2CC |= 0x400;
        a->flags2D0 |= 0xA0000;
        SceneActorAttachment_ReplaceResponseSlot((u8 *)a, 0, 0x7F, 0);
        SceneActorAttachment_ReplaceResponseSlot((u8 *)a, 1, 0x7F, 0);
        SceneActorAttachment_ReplaceResponseSlot((u8 *)a, 3, 0x7F, 0);
        SceneActorEffects_AdvanceSteps((u8 *)a, 4);
        SetReactionState(a, 3, 0, 0, 0x11, a->deathControl);
        if (SceneActorDamage_ShouldShow(a))
            ShowDamage(a);
        if (!SceneActorDamage_TestFlags(gEngineFlags, 0x4800) && !(a->flags2D0 & 0x800))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)a->body.id);
        if (category == 4 || category == 1) {
            union SceneActorRegistryPackedPosition range;
            range.packed.x = 80;
            range.packed.y = 60;
            range.packed.z = 60;
            SceneActorEffects_SpawnDefeatSparks(a, &range.position, gRandomTable,
                                                SceneActorEffects_NextSparkCount(gRandomTable));
            switch (a->category) {
                case 4:
                case 1:
                case 5:
                case 7:
                case 6:
                case 24:
                case 25:
                    Sound_PlaySong(0xE5);
                    break;
            }
        }
    } else {
        s32 half = a->health / 2;
        s32 left;
        a->reactionSoundVariant = half;
        *(s16 *)((u8 *)&a->colliderA + 0x3E) = half;
        left = a->health - half;
        if (left <= 0)
            left = 0;
        a->health = left;
        if (SceneActorDamage_ShouldShow(a))
            ShowDamage(a);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_ApplyHitResponse);
#endif
