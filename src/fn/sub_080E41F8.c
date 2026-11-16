#include "scene_actor_chase.h"
struct DogAttackContext {
    u8 unknown0[0x624];
    u8 collider[0x54];
    u32 flags;
    u8 unknown67C[0x1C];
    u16 timer698, timer69A, timer69C, timer69E;
    u8 unknown6A0[7];
    u8 emitted;
};
#define FIELD(p, n, T) (*(T *)((u8 *)(p) + (n)))
void Dog_UpdateLeapWindup(void);
void Dog_SelectRetreatOrChase(struct ChaseActor *);
static inline void And32(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline void Or16(u16 *p, u32 mask)
{
    u32 value = mask;
    value |= *p;
    *p = value;
}
static inline void Dog_MaskAttackFlags(u32 *field, u32 mask)
{
    *field &= mask;
}
static inline void Dog_ClearAttackFlags(struct DogAttackContext *ctx, u32 bits)
{
    Dog_MaskAttackFlags(&ctx->flags, ~bits);
}

void Dog_UpdateLeapAttack(struct ChaseActor *a)
{
    struct DogAttackContext *context = a->base.context;
    if (Chase_TakeArrived(a)) {
        And32(&context->flags, ~8);
        SceneActorReaction_EnableMotionFlags(0x100, &a->base.flags2D4);
        {
            void *callback = Dog_UpdateLeapWindup;
            u32 kind = 34;
            a->base.field2B2 = 1;
            a->base.field2B0 = 0;
            a->base.field2B5 = kind;
            a->base.target = callback;
            a->base.field2C8 = 0;
            {
                u32 *flags = &a->base.flags114;
                s32 mask = -2;
                *flags &= mask;
            }
            a->base.word104 = 0;
        }
        Dog_ClearAttackFlags(a->base.context, 2);
        {
            void *collider = context->collider;
            u32 damage = a->base.attacks.first[1];
            u32 response = a->base.attacks.second[1];
            u32 power = a->base.attacks.flags[1];
            u32 extra = a->base.attacks.parameter[1];
            Collider_SetAttackProperties(collider, damage, response, 0, power, extra);
        }
        context->emitted = 0;
        context->timer698 = context->timer69C;
    }
    if (a->base.field2B0) {
        context->timer698 = context->timer69C;
        {
            u16 *flags = &a->base.flags2D4;
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
        Dog_SelectRetreatOrChase(a);
    } else
        a->base.counter2AC++;
}
