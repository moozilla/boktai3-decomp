#include "global.h"

struct SceneActorCollisionParameters {
    u16 field3C[4], field40[4], field44[4];
    u32 reactionFlags[4];
};
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

struct AxeAttackContext {
    u8 unk_00[0xCEC];
    u8 attackColliders[2][0x54];
    u8 unk_D94[0xE4C - 0xD94];
    u32 flags;
};
struct AxeAttackActor {
    u8 unk_00[0xB4];
    struct SceneActorCollisionParameters attacks;
    u8 unk_DC[0x188 - 0xDC];
    u8 collider[0x54];
    u8 unk_1DC[0x3D0 - 0x1DC];
    struct AxeAttackContext *context;
};
static inline u8 TestFlag(u32 mask, volatile u32 *flags)
{
    if (*flags & mask)
        return 1;
    return 0;
}
#define ApplyAttack(collider, actor, index)                                                        \
    do {                                                                                           \
        u16 power = (actor)->attacks.field3C[index];                                               \
        u16 force = (actor)->attacks.field40[index];                                               \
        u32 flags = (actor)->attacks.reactionFlags[index];                                         \
        u16 duration = (actor)->attacks.field44[index];                                            \
        Collider_SetAttackProperties(collider, power, force, 0, flags, duration);                  \
    } while (0)

#ifdef NONMATCHING
// fuzzy: 97.4%
// scope shape 2 (differences once low registers are set aside)
// confidence=low: fires the axe's attack on the actor collider and both attack colliders, with attack parameters chosen by the context's flags.
void sub_080FCDEC(struct AxeAttackActor *actor)
{
    struct AxeAttackContext *context = actor->context;
    u32 *flags;

    if (TestFlag(0x200, &context->flags)) {
        ApplyAttack(actor->collider, actor, 2);
        ApplyAttack(context->attackColliders[0], actor, 2);
        ApplyAttack(context->attackColliders[1], actor, 2);
    } else {
        u32 mask = 0x800;
        flags = &context->flags;
        if (TestFlag(mask, flags)) {
            s32 power = actor->attacks.field3C[2] * 2;
            s32 force = actor->attacks.field40[2] * 2;
            {
                u32 reactionFlags = actor->attacks.reactionFlags[2];
                u16 duration = actor->attacks.field44[2];
                Collider_SetAttackProperties(actor->collider, power, force, 0, reactionFlags,
                                             duration);
            }
            {
                u32 reactionFlags = actor->attacks.reactionFlags[2];
                u16 duration = actor->attacks.field44[2];
                Collider_SetAttackProperties(context->attackColliders[0], power, force, 0,
                                             reactionFlags, duration);
            }
            {
                u32 reactionFlags = actor->attacks.reactionFlags[2];
                u16 duration = actor->attacks.field44[2];
                Collider_SetAttackProperties(context->attackColliders[1], power, force, 0,
                                             reactionFlags, duration);
            }
        } else if (TestFlag(0x400, flags)) {
            ApplyAttack(actor->collider, actor, 1);
            ApplyAttack(context->attackColliders[0], actor, 1);
            ApplyAttack(context->attackColliders[1], actor, 1);
        } else if (TestFlag(0x1000, flags)) {
            ApplyAttack(actor->collider, actor, 0);
            ApplyAttack(context->attackColliders[0], actor, 0);
            ApplyAttack(context->attackColliders[1], actor, 0);
        } else {
            ApplyAttack(actor->collider, actor, 0);
            ApplyAttack(context->attackColliders[0], actor, 0);
            ApplyAttack(context->attackColliders[1], actor, 0);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080FCDEC);
#endif
