#include "global.h"
// confidence=low: switches between reaction setup and directed secondary motion.
#define SceneActorReaction_SetMotionMode sub_08080BE0
#ifdef NONMATCHING
// fuzzy: 90.8%
// fix.py best (sweep after the inlined-insn fix)
// Flag-width, expression ordering and register allocation differences remain.
#include "scene_actor_chase.h"

struct ReactionOwner {
    u8 unknown0[0x3C];
    struct SceneActorRegistryResetView *actor;
};
extern const s16 gUnk_086149C4[];
static inline u32 Reaction_MotionTest(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u32 Reaction_WordTest(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u8 Reaction_IsBlocked(struct SceneActorRegistryResetView *actor)
{
    if (!actor)
        return 0;
    if (Reaction_MotionTest(actor, 0x40))
        return 1;
    if (Reaction_WordTest(actor, 0x20000))
        return 1;
    if (actor->health <= 0)
        return 1;
    return 0;
}
static inline u8 Reaction_IsBlockedFlags(struct SceneActorRegistryResetView *actor, u32 flags)
{
    if (!actor)
        return 0;
    if (flags & 0x40)
        return 1;
    if (Reaction_WordTest(actor, 0x20000))
        return 1;
    if (actor->health <= 0)
        return 1;
    return 0;
}
static inline void Reaction_ClearHalf(u16 *field, s32 mask)
{
    *field &= mask;
}
static inline void Reaction_ClearWord(u32 *field, u32 bits)
{
    u32 mask = ~bits;
    *field &= mask;
}
static inline void Reaction_SetHalf(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D6 = mask | actor->flags2D6;
}
static inline s32 Reaction_ScaleMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
inline static void Reaction_ClearWord_08080BE0(u32 *field, u32 bits)
{
    u16 mask = ~bits;
    *field &= mask;
}

inline static void Reaction_SetHalf_08080BE0(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2D6 = mask | actor->flags2D6;
}

u32 SceneActorReaction_SetMotionMode(struct ReactionOwner *owner, s32 mode, s32 amplitude)
{
    struct SceneActorRegistryResetView *actor = owner->actor;
    u8 *group = (u8 *)(&actor->group);
    struct ContactCollider *second = &actor->colliderB;
    struct ContactCollider *first = &actor->colliderA;
    if (Reaction_IsBlocked(actor))
        return 0;
    switch (mode) {
        case 0: {
            u16 bit = 0x8000;
            u16 *motion = &actor->flags2D4;
            if (!((*motion) & bit)) {
                second->flags |= 4;
                first->flags |= 4;
                Chase_SetCtl(actor, *((void **)(group + 0x23C)), 0x1F);
                Chase_SetState(actor, 3, 0, 0);
                *motion |= bit;
                if (*motion) {
                    if (Reaction_IsBlockedFlags(actor, *motion)) {
                        *motion = 0x40;
                        actor->field2E6 = 0;
                        Reaction_ClearHalf(&actor->flags2D6, ~0x980);
                    } else if ((*motion) & 0x803E) {
                        *motion &= ~0x201;
                        Reaction_ClearWord_08080BE0(&actor->flags2CC, 0x4000);
                        actor->tracking.flags &= ~0x10;
                        if (!((*motion) & 0x20))
                            Reaction_ClearWord_08080BE0(&actor->flags2CC, 0x80);
                        Reaction_ClearWord_08080BE0(&actor->flags2D0, 0x100004);
                        Reaction_SetHalf_08080BE0(actor, 0x400);
                        actor->field2E6 = 0;
                        actor->flags2CC |= 0x2000;
                    } else
                        Reaction_ClearWord_08080BE0(&actor->flags2CC, 0x2000);
                } else
                    Reaction_ClearWord_08080BE0(&actor->flags2CC, 0x2000);
            }
            actor->secondaryMotion.x = 0;
            actor->secondaryMotion.y = 0;
            actor->secondaryMotion.z = 0;
            first->responseParameter = 0;
            if (amplitude) {
                actor->reactionSoundVariant = amplitude;
                actor->field2E6 = 1;
                actor->field2E0 = 1;
                actor->field2DC = 2;
                actor->field2D8 = 1;
                Reaction_SetHalf_08080BE0(actor, 0x20);
            } else {
                actor->reactionSoundVariant = 0;
                actor->field2E6 = 0;
                actor->field2E0 = 0;
                actor->field2DC = 0;
                actor->field2D8 = 0;
            }
            break;
        }

        case 1:
            Reaction_ClearHalf(&second->flags, ~4);
            Reaction_ClearHalf(&first->flags, ~4);
            Reaction_ClearHalf(&actor->flags2D4, ~0x8000);
            {
                u32 direction = actor->body.direction;
                struct ContactPosition *motion = &actor->secondaryMotion;
                motion->x = Reaction_ScaleMotion(gUnk_086149C4[(direction + 64) & 255] * amplitude);
                motion->y = 0;
                motion->z = Reaction_ScaleMotion(gUnk_086149C4[direction] * amplitude);
            }
            actor->secondaryMotion.y = 4;
            Chase_SetState(actor, 3, 0, 0);
            Chase_SetCtl(actor, *((void **)(group + 0x240)), 0x20);
            break;
    }

    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08080BE0);
#endif
