#include "global.h"
// confidence=low: runs a timed reaction burst and restores actor control on expiry.
#define SceneActorReaction_UpdateTimedBurst sub_08081C98
#ifdef NONMATCHING
// fuzzy: 66.6%
// Complete timing, palette and particle paths; loop allocation and field lifetimes differ.
#include "scene_actor_chase.h"
#include "scene_actor_reaction.h"
#include "engine.h"
void EffectGroup_Reset(void *);
s32 EffectGroup_SpawnPart(void *, u32, const struct ContactPosition *,
                          const struct ContactPosition *, const struct ContactPosition *, u32, u32);
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
static inline void Reaction_SetHalf(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D6 = mask | actor->flags2D6;
}
static inline u32 Reaction_Flags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline u32 Reaction_Random(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
static inline void Reaction_SpawnPart(u8 *group)
{
    struct ContactPosition velocity, scale, position;
    const struct ContactPosition *source;
    const u16 *table = gRandomTable;
    velocity.x = (Reaction_Random(table) & 15) - 7;
    velocity.y = (Reaction_Random(table) & 31) - 16;
    velocity.z = (Reaction_Random(table) & 15) - 7;
    scale.x = 5;
    scale.y = 10;
    scale.z = 5;
    source = *(struct ContactPosition **)(group + 0xB0);
    position.x = source->x;
    position.y = source->y + 128;
    position.z = source->z;
    EffectGroup_SpawnPart(group + 0xA8, 3, &position, &velocity, &scale, 40, 40);
}
static inline s32 Reaction_ThirtySecond(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
void SceneActorReaction_UpdateTimedBurst(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    u32 duration;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorReactionMotion *reaction;
        struct SceneActorRegistryControl *control;
        u32 *flags;
        u32 *word;
        u16 *motion;
        u32 bank;
        actor->body.direction = (((actor->body.direction + 32) >> 6) & 3) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        actor->field2C2 = 64;
        reaction = (void *)((u8 *)actor + 0xDC);
        Reaction_ClearMotion(reaction, ~0x18);
        word = &actor->flags2D0;
        Reaction_ClearWord(word, ~0x1000000);
        actor->field2C2 = 64;
        *(u16 *)((u8 *)actor + 0x414) = 0;
        *(u16 *)((u8 *)actor + 0x418) = 0;
        motion = &actor->flags2D4;
        Reaction_ClearHalf(motion, ~0x380);
        Reaction_ClearMotion(reaction, ~0x18);
        control = (void *)((u8 *)actor + 0x420);
        if (Reaction_Flags(actor, 0x200000)) {
            Reaction_ClearWord(word, ~0x1000000);
            Reaction_ClearHalf(motion, ~0x200);
            *(u8 *)((u8 *)control + 0xA) = 0;
            control->counter = 0;
            control->timer = 0;
        }
        Reaction_ClearHalf(motion, ~0x400);
        *(u8 *)((u8 *)actor + 0x5DA) = 255;
        flags = &actor->flags2CC;
        control = (void *)(group + 0x4C);
        Reaction_ClearWord(flags, ~0x141C0);
        control->counter = 0;
        *(u16 *)((u8 *)control + 0x14) = 0;
        control->timer = 0;
        EffectGroup_Reset(group + 0xA8);
        SceneActorEffects_EmitRandomOffsets(actor, 3, 2, 10, 20, 3600);
        Reaction_SetWord(flags, 0x2000000);
        *word = (*word | 0x4000) & ~0x140;
        if (*word & 0x4000)
            Reaction_Palette(actor, *(u16 *)((u8 *)actor + 0x1DC));
        else
            Reaction_Palette(actor, *(u16 *)((u8 *)actor + 0x1E4));
    }
    duration = *(u16 *)(group + 0x1FA);
    if (duration * 2 == actor->field2B8 || duration == actor->field2B8) {
        Reaction_SpawnPart(group);
        Reaction_SetHalf(actor, 0x20);
    }
    if (actor->field2B8 <= 10)
        Reaction_SetHalf(actor, 0x20);
    actor->field1EA = 65;
    if (!actor->field2B8) {
        struct ContactCollider *collider = &actor->colliderA;
        u32 mask = 0x100;
        collider->flags = mask | collider->flags;
        if (Chase_MotionFlags(actor, 0x2000)) {
            s32 left = 2;
            do {
                Reaction_SpawnPart(group);
            } while (--left >= 0);
            actor->fieldAA = 0;
            actor->field2B8 = 0;
        }
        Reaction_ClearHalf(&actor->flags2D4, ~0x2000);
        Chase_SetState(actor, 3, 1, 0);
        Chase_SetCtl(actor, *(void **)(group + 0x210), 15);
        actor->field2B6 = (Reaction_ThirtySecond((actor->body.direction + 128) & 255) + 7) & 7;
    } else {
        actor->field2B8--;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08081C98);
#endif
