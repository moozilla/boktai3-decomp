#include "global.h"
// confidence=low: runs a timed reaction burst and restores actor control on expiry.
#include "scene/motion/scene_actor_chase.h"
#include "scene/actor/scene_actor_reaction.h"
#include "engine/engine.h"
#include "scene/actor/scene_actor_part_burst.h"
#include "scene/collision/scene_actor_contact_response.h"
#include "engine/word_flags.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/actor/scene_actor_init_actor.h"
#include "ui/hud/damage_number_display.h"
void EffectGroup_Reset(void *);
void SceneActorEffects_EmitRandomOffsets(void *, s32, u32, s32, s32, u32);
static inline u8 Reaction_HasFlags(u32 bits)
{
    u8 flag = 0;
    if (bits)
        flag = 1;
    return flag;
}

static inline void Reaction_Palette(struct ChaseActor *actor, u32 bank)
{
    if (!actor->base.drawableKind)
        AnimData_SelectPaletteBank(&((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->graphics, bank);
    else
        ((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile.palette =
            (u16 *)(gPaletteBankData + bank * 32);
}
static inline void Reaction_SetWord(u32 *flags, u32 mask)
{
    *flags = mask | *flags;
}
static inline void Reaction_ClearWord(u32 *field, s32 mask)
{
    *field &= mask;
}
static inline void Reaction_ClearMotion(struct SceneActorChaseTracking *motion, s32 mask)
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
static inline void Reaction_SpawnPart(struct SceneActorChaseGroup *group)
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
    source = group->effectOrigin;
    position.x = source->x;
    position.y = source->y + 128;
    position.z = source->z;
    EffectGroup_SpawnPart(group->effectConfig, 3, &position, &velocity, &scale, 40, 40);
}
void SceneActorReaction_UpdateTimedBurst(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 duration;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorChaseTracking *reaction;
        struct SceneActorRegistryControl *control;
        u32 *flags;
        u32 *word;
        u16 *motion;
        u32 bit;
        actor->base.body.direction = (((MonsterInit_GetDirection(actor) + 32) >> 6) & 3) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        actor->base.field2C2 = 64;
        reaction = &actor->base.tracking;
        Reaction_ClearMotion(reaction, ~0x18);
        word = &actor->base.statusFlags;
        Reaction_ClearWord(word, ~0x1000000);
        actor->base.field2C2 = 64;
        actor->group.common.gate.flags = 0;
        actor->group.common.gate.counter = 0;
        motion = &actor->base.motionFlags;
        HalfwordFlags_And(motion, ~0x380);
        Reaction_ClearMotion(reaction, ~0x18);
        control = &actor->group.common.control;
        bit = 0x200000;
        flags = &actor->base.behaviorFlags;
        if (Reaction_HasFlags(*flags & bit)) {
            Reaction_ClearWord(word, ~0x1000000);
            HalfwordFlags_And(motion, ~0x200);
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        HalfwordFlags_And(motion, ~0x400);
        actor->group.turn.turnsRemaining = -1;
        Reaction_ClearRoute(flags, &group->common.control);
        EffectGroup_Reset(group->effectConfig);
        SceneActorEffects_EmitRandomOffsets(actor, 3, 2, 10, 20, 3600);
        Reaction_SetWord(flags, 0x2000000);
        {
            u32 shown = 0x4000;
            *word = (shown | *word) & ~0x140;
            if (Reaction_HasFlags(*word & shown))
                Reaction_Palette(actor, actor->base.alternatePalette);
            else
                Reaction_Palette(actor, actor->base.alternateOffset);
        }
    }
    duration = group->hitBurstInterval;
    if (duration * 2 == actor->base.hitLockTimer || duration == actor->base.hitLockTimer) {
        Reaction_SpawnPart(group);
        SceneActorChase_OrReactionFlags(actor, 0x20);
    }
    if (actor->base.hitLockTimer <= 10)
        SceneActorChase_OrReactionFlags(actor, 0x20);
    {
        u32 timer = 65;
        actor->base.paletteTimer = timer;
    }
    if (!actor->base.hitLockTimer) {
        struct ContactCollider *collider = &actor->base.colliderA;
        u32 mask = 0x100;
        collider->flags = mask | collider->flags;
        if (Reaction_HasFlags(SceneActorContact_MotionFlags(actor, 0x2000)))
            SceneActorEffects_SpawnContactParts((void *)actor);
        SceneActorChase_AndMotionFlags(actor, ~0x2000);
        Chase_SetState(actor, 3, 1, 0);
        Chase_SetCtl(actor, group->routeResumeControl, 15);
        actor->base.reactionDirection =
            (DamageNumber_ProjectScale((MonsterInit_GetDirection(actor) + 128) & 255) + 7) & 7;
    } else {
        actor->base.hitLockTimer--;
    }
}
