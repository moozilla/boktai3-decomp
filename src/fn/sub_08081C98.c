#include "global.h"
// confidence=low: runs a timed reaction burst and restores actor control on expiry.
#ifdef NONMATCHING
// fuzzy: 94.3%
// 36 insns differ, all stack-slot order of the PRE pseudos (and frame 0x88 vs 0x84): the code
// is otherwise identical. `scope.py preorder` puts the ROM's slot order at T = 267 buckets, so
// gcse's input needs N = 532..535 real insns (this draft has 510). Narrow temporaries, casts and
// helper-local copies all fold before gcse; insns that only flow or jump2 remove would add N.
#include "scene_actor_chase.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_actor_part_burst.h"
void EffectGroup_Reset(void *);
void SceneActorEffects_EmitRandomOffsets(void *, s32, u32, s32, s32, u32);
static inline void Reaction_Palette(struct SceneActorRegistryResetView *actor, u32 bank)
{
    if (!actor->drawableKind)
        AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, bank);
    else
        *(u8 **)((u8 *)actor->drawable + 0x68) = gPaletteBankData + bank * 32;
}
static inline void Reaction_SetWord(u32 *flags, u32 mask)
{
    *flags = mask | *flags;
}
static inline void Reaction_ClearHalf(u16 *field, s32 mask)
{
    *field &= mask;
}
static inline void Reaction_ClearWord(u32 *field, s32 mask)
{
    *field &= mask;
}
static inline void Reaction_ClearMotion(struct SceneActorReactionMotion *motion, s32 mask)
{
    motion->flags &= mask;
}
static inline void Reaction_ClearRoute(u32 *flags, struct SceneActorRegistryControl *control)
{
    Reaction_ClearWord(flags, ~0x141C0);
    control->counter = 0;
    control->field14 = 0;
    control->timer = 0;
}
static inline u32 Reaction_Motion(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline void Reaction_ClearMotionBits(struct SceneActorRegistryResetView *actor, s32 mask)
{
    actor->flags2D4 &= mask;
}
static inline void Reaction_SetHalf(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2D6 = mask | actor->flags2D6;
}
static inline u32 Reaction_Flags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline void Reaction_SpawnPart(u8 *group)
{
    struct ContactPosition velocity, scale, position;
    const struct ContactPosition *source;
    const u16 *table = gRandomTable;
    velocity.x = (SceneActorEffects_NextSample(table) & 15) - 7;
    velocity.y = (SceneActorEffects_NextSample(table) & 31) - 16;
    velocity.z = (SceneActorEffects_NextSample(table) & 15) - 7;
    scale.x = 5;
    scale.y = 10;
    scale.z = 5;
    source = *(const struct ContactPosition **)(group + 0xB0);
    position.x = source->x;
    position.y = source->y + 128;
    position.z = source->z;
    EffectGroup_SpawnPart((void *)(group + 0xA8), 3, &position, &velocity, &scale, 40, 40);
}
static inline s32 Reaction_ThirtySecond(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
void SceneActorReaction_UpdateTimedBurst(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    u32 duration;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorReactionMotion *reaction;
        struct SceneActorRegistryControl *control;
        u32 *flags;
        u32 *word;
        u16 *motion;
        u32 bit;
        actor->body.direction = (((actor->body.direction + 32) >> 6) & 3) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        actor->field2C2 = 64;
        reaction = (void *)&actor->tracking.target;
        Reaction_ClearMotion(reaction, ~0x18);
        word = &actor->flags2D0;
        Reaction_ClearWord(word, ~0x1000000);
        actor->field2C2 = 64;
        actor->group.common.gate.flags = 0;
        actor->group.common.gate.counter = 0;
        motion = &actor->flags2D4;
        Reaction_ClearHalf(motion, ~0x380);
        Reaction_ClearMotion(reaction, ~0x18);
        control = &actor->group.common.control;
        bit = 0x200000;
        flags = &actor->flags2CC;
        if (*flags & bit) {
            Reaction_ClearWord(word, ~0x1000000);
            Reaction_ClearHalf(motion, ~0x200);
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        Reaction_ClearHalf(motion, ~0x400);
        actor->group.turn.turnsRemaining = -1;
        Reaction_ClearRoute(flags, (void *)(group + 0x4C));
        EffectGroup_Reset(group + 0xA8);
        SceneActorEffects_EmitRandomOffsets(actor, 3, 2, 10, 20, 3600);
        Reaction_SetWord(flags, 0x2000000);
        {
            u32 shown = 0x4000;
            *word = (shown | *word) & ~0x140;
            if (*word & shown)
                Reaction_Palette(actor, actor->alternatePalette);
            else
                Reaction_Palette(actor, actor->alternateOffset);
        }
    }
    duration = *(u16 *)(group + 0x1FA);
    if (duration * 2 == actor->field2B8 || duration == actor->field2B8) {
        Reaction_SpawnPart(group);
        Reaction_SetHalf(actor, 0x20);
    }
    if (actor->field2B8 <= 10)
        Reaction_SetHalf(actor, 0x20);
    {
        u32 timer = 65;
        actor->field1EA = timer;
    }
    if (!actor->field2B8) {
        struct ContactCollider *collider = &actor->colliderA;
        u32 mask = 0x100;
        collider->flags = mask | collider->flags;
        if (Reaction_Motion(actor, 0x2000))
            SceneActorEffects_SpawnContactParts((void *)actor);
        Reaction_ClearMotionBits(actor, ~0x2000);
        Chase_SetState(actor, 3, 1, 0);
        Chase_SetCtl(actor, *(void **)(group + 0x210), 15);
        actor->field2B6 = (Reaction_ThirtySecond((actor->body.direction + 128) & 255) + 7) & 7;
    } else {
        actor->field2B8--;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_UpdateTimedBurst);
#endif
