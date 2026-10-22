#include "scene_actor_chase.h"
#ifdef NONMATCHING
// fuzzy: 80.7%
// Remaining differences: register allocation and address/argument evaluation order.
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
void SceneActorAnimation_EmitDirectionalTrail(void);
void Dog_SelectRetreatOrChase(struct SceneActorRegistryResetView *);
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
void Dog_UpdateTrailAttack(struct SceneActorRegistryResetView *a)
{
    struct DogAttackContext *context = a->context;
    if (Chase_TakeArrived(a)) {
        u8 zero8;
        u32 zero;
        And32(&context->flags, ~8);
        {
            u32 bit = 0x100;
            u16 *flags = &a->flags2D4;
            u32 value = *flags;
            zero8 = 0;
            zero = 0;
            *flags = bit | value;
        }
        {
            void *callback = SceneActorAnimation_EmitDirectionalTrail;
            u32 kind = 37;
            a->field2B2 = 1;
            a->field2B0 = zero8;
            a->field2B5 = kind;
            a->target = callback;
            a->field2C8 = zero;
            And32(&a->flags114, ~1);
            a->word104 = zero;
        }
        {
            u32 *p = (void *)((u8 *)a->context + 0x678);
            u32 bit = 2;
            *p |= bit;
        }
        Collider_SetAttackProperties(context->collider, a->fieldB8, a->fieldC0, 0, a->wordD4,
                                     a->fieldC8);
        context->emitted = zero8;
    }
    if (a->field2B0) {
        context->timer69A = context->timer69E;
        {
            u16 *flags = &a->flags2D4;
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
        Dog_SelectRetreatOrChase(a);
    } else
        a->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", Dog_UpdateTrailAttack);
#endif
