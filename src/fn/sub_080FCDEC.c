#include "scene_actor_state.h"

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

struct AxeAttackContext {
    u8 unk_00[0xCEC];
    struct ContactCollider attackColliders[2];
    u8 unk_D94[0xE4C - 0xD94];
    u32 flags;
};
static inline u8 Axe_TestFlag(u32 mask, const u32 *flags)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline u8 Axe_TestMaskedFlags(u32 value)
{
    if (value)
        return 1;
    return 0;
}
static inline u32 Axe_ReadMaskedFlags(u32 mask, const u32 *flags)
{
    return *flags & mask;
}
struct AxeAttackSelection {
    u32 *flags;
};
static inline u8 Axe_TestSelectionFlags(u32 mask, const struct AxeAttackSelection *selection)
{
    return Axe_TestFlag(mask, selection->flags);
}
#define Axe_ApplyAttack(collider, actor, index)                                                    \
    do {                                                                                           \
        u16 power = (actor)->attacks.first[index];                                                 \
        u16 force = (actor)->attacks.second[index];                                                \
        u32 flags = (actor)->attacks.flags[index];                                                 \
        u16 duration = (actor)->attacks.parameter[index];                                          \
        Collider_SetAttackProperties(collider, power, force, 0, flags, duration);                  \
    } while (0)

// confidence=low: fires the axe's attack on the actor collider and both attack colliders, with attack parameters chosen by the context's flags.
void sub_080FCDEC(struct SceneActorRegistryResetView *actor)
{
    struct AxeAttackContext *context = actor->context;
    struct AxeAttackSelection selection;

    if (Axe_TestFlag(0x200, &context->flags)) {
        Axe_ApplyAttack(&actor->colliderB, actor, 2);
        Axe_ApplyAttack(&context->attackColliders[0], actor, 2);
        Axe_ApplyAttack(&context->attackColliders[1], actor, 2);
    } else {
        u32 mask = 0x800;
        u32 value = Axe_ReadMaskedFlags(mask, &context->flags);
        selection.flags = &context->flags;
        if (Axe_TestMaskedFlags(value)) {
            s32 power = actor->attacks.first[2] * 2;
            s32 force = actor->attacks.second[2] * 2;
            {
                u32 reactionFlags = actor->attacks.flags[2];
                u16 duration = actor->attacks.parameter[2];
                Collider_SetAttackProperties(&actor->colliderB, power, force, 0, reactionFlags,
                                             duration);
            }
            {
                u32 reactionFlags = actor->attacks.flags[2];
                u16 duration = actor->attacks.parameter[2];
                Collider_SetAttackProperties(&context->attackColliders[0], power, force, 0,
                                             reactionFlags, duration);
            }
            {
                u32 reactionFlags = actor->attacks.flags[2];
                u16 duration = actor->attacks.parameter[2];
                Collider_SetAttackProperties(&context->attackColliders[1], power, force, 0,
                                             reactionFlags, duration);
            }
        } else if (Axe_TestSelectionFlags(0x400, &selection)) {
            Axe_ApplyAttack(&actor->colliderB, actor, 1);
            Axe_ApplyAttack(&context->attackColliders[0], actor, 1);
            Axe_ApplyAttack(&context->attackColliders[1], actor, 1);
        } else if (Axe_TestSelectionFlags(0x1000, &selection)) {
            Axe_ApplyAttack(&actor->colliderB, actor, 0);
            Axe_ApplyAttack(&context->attackColliders[0], actor, 0);
            Axe_ApplyAttack(&context->attackColliders[1], actor, 0);
        } else {
            Axe_ApplyAttack(&actor->colliderB, actor, 0);
            Axe_ApplyAttack(&context->attackColliders[0], actor, 0);
            Axe_ApplyAttack(&context->attackColliders[1], actor, 0);
        }
    }
}
