#include "global.h"
// confidence=low: settles reaction displacement and restores normal actor control.
#define SceneActorReaction_SettleDisplacement sub_0808170C
#ifdef NONMATCHING
// fuzzy: 74.8%
// fix.py best (sweep after the inlined-insn fix)
// Full flow recovered; reset-field and motion-pointer lifetimes still differ.
#include "scene_actor_chase.h"
#include "scene_actor_reaction.h"

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u8);
struct ReactionFollowMotion {
    struct ContactPosition target;
    struct ContactPosition current;
};
struct ReactionAnimationStatus {
    u8 unknown0[7];
    u8 active;
    u16 mode, unknownA, unknownC, frame;
};
static inline u32 Reaction_WordFlags(u32 *field, u32 mask)
{
    return *field & mask;
}
static inline u32 Reaction_ActorWord(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u32 Reaction_ActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline void Reaction_SetMotion(struct SceneActorRegistryResetView *actor, u32 bits, s32 mask)
{
    actor->flags2D4 = (bits | actor->flags2D4) & mask;
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
static inline s32 Reaction_Quarter(s32 value)
{
    if (value >= 0)
        return value >> 2;
    return -((-value) >> 2);
}
static inline s32 Reaction_ThirtySecond(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
static inline s32 Reaction_ClampMotion(s32 value)
{
    if (value > 200)
        return 200;
    if (value < -200)
        return -200;
    return value;
}
static inline u8 Reaction_MotionStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline u8 Reaction_Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void SceneActorReaction_SettleDisplacement(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)(&actor->group);
    s32 counter;
    struct ReactionFollowMotion *follow;
    s32 x;
    s32 z;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 category = actor->category;
        u32 *word;
        u32 original;
        u16 *motion;
        struct SceneActorReactionMotion *reaction;
        struct SceneActorRegistryControl *control;
        u32 *flags;
        actor->direction = (((actor->direction + 32) >> 6) & 3) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 6);
        original = Reaction_ActorWord(actor, ~0U);
        word = &actor->flags2D0;
        word = &actor->flags2D0;
        if (!(original & 2)) {
            if (original & 1) {
                SceneActorAttachment_ReplaceResponseSlot(actor, 3, 2, category);
                Reaction_ClearWord(word, ~1);
            } else
                SceneActorAttachment_ReplaceResponseSlot(actor, 3, 0, category);
            *word |= 2;
            switch (category) {
                case 4:
                    Sound_PlaySong(237);
                    break;

                case 1:
                    Sound_PlaySong(254);
                    break;

                case 3:
                    SceneActorPlacement_PlayVariantEventSound((void *)actor, 2);
                    break;

                case 25:
                    Sound_PlaySong(460);
                    break;

                case 5:

                case 6:

                case 7:
                    Sound_PlaySong(316);
                    break;

                case 21:
                    Sound_PlaySong(316);
                    break;
            }
        }
        Reaction_SetMotion(actor, 2, ~0x17A9);
        motion = &actor->flags2D4;
        actor->field2C2 = 64;
        *((u16 *)(((u8 *)actor) + 0x414)) = 0;
        *((u16 *)(((u8 *)actor) + 0x418)) = 0;
        Reaction_ClearHalf(motion, ~0x380);
        reaction = (void *)(((u8 *)actor) + 0xDC);
        Reaction_ClearMotion(reaction, ~0x18);
        control = (void *)(((u8 *)actor) + 0x420);
        if (Reaction_ActorFlags(actor, 0x200000)) {
            Reaction_ClearWord(word, ~0x1000000);
            Reaction_ClearHalf(motion, ~0x200);
            *((u8 *)(((u8 *)control) + 0xA)) = 0;
            control->counter = 0;
            control->timer = 0;
        }
        flags = &actor->flags2CC;
        Reaction_ClearHalf(motion, ~0x400);
        *((u8 *)(group + 0x206)) = 255;
        Reaction_ClearWord(flags, ~0x80);
        actor->field2C2 = 64;
        Reaction_ClearMotion(reaction, ~0x18);
        Reaction_ClearWord(word, ~0x1000000);
        control = (void *)(group + 0x4C);
        Reaction_ClearWord(flags, ~0x141C0);
        control->counter = 0;
        *((u16 *)(((u8 *)control) + 0x14)) = 0;
        control->timer = 0;
    }
    if (!actor->drawableKind) {
        struct ReactionAnimationStatus *status = (void *)(((u8 *)actor->drawable) + 0x48);
        if (((status->mode == 2) && (status->frame == (!(!status->active)))) &&
            (status->active != 1))
            status->frame = 0;
    }
    counter = *((s16 *)(&actor->fieldAA));
    if (((u32)(counter - 30)) <= 89) {
        s32 remainder = Mod(counter, 30);
        s32 limit = 15 - Div(counter, 10);
        if (remainder < limit) {
            u32 mask = 0x20;
            actor->flags2D6 = mask | actor->flags2D6;
        }
    }
    follow = (void *)(((u8 *)actor) + 0x310);
    x = Reaction_Quarter(follow->target.x - follow->current.x);
    z = Reaction_Quarter(follow->target.z - follow->current.z);
    x = Reaction_ClampMotion(x);
    z = Reaction_ClampMotion(z);
    actor->motion.x += x;
    actor->motion.z += z;
    follow->current.x += x;
    follow->current.z += z;
    if (Reaction_MotionStopped(x, z) && (counter <= 0)) {
        struct ContactCollider *collider;
        actor->fieldAA = 0;
        actor->field2B6 = (Reaction_ThirtySecond(actor->direction) + 7) & 7;
        Chase_SetState(actor, 3, 1, 0);
        Chase_SetCtl(actor, *((void **)(group + 0x20C)), 8);
        Reaction_ClearWord(&actor->flags2D0, ~2);
        Reaction_ClearHalf(&actor->flags2D4, ~0x3A);
        Chase_SetState(actor, 3, 1, 0);
        actor->direction = ((actor->direction + 32) >> 6) << 6;
        collider = &actor->colliderA;
        if (Reaction_Test(actor->flags2C0, 0x40)) {
            collider->flags &= 0xFEFF;
        } else if (Chase_MotionFlags(actor, 0x2016)) {
            collider->flags &= 0xFEFF;
        } else {
            u32 old = collider->flags;
            u32 mask = 0x100;
            collider->flags = mask | old;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0808170C);
#endif
