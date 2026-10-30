#include "scene_actor_chase.h"
// MATCHING_CONCESSION: register-pinning
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
void Dog_UpdateLeapAttack(struct SceneActorRegistryResetView *a)
{
    struct DogAttackContext *context = a->context;
    if (Chase_TakeArrived(a)) {
        u32 zero8;
        register u32 zero asm("r4");
        {
            register u32 offset asm("r0") = 0x678;
            u32 *p;
            p = (u32 *)((u8 *)context + offset);
            And32(p, ~8);
        }
        {
            u32 bit = 0x100;
            register u16 *flags asm("r2") = &a->flags2D4;
            register u32 value asm("r1") = *flags;
            zero8 = 0;
            zero = 0;
            asm("" : "+r"(zero8), "+r"(zero), "+r"(bit), "+r"(flags), "+r"(value));
            *flags = bit | value;
        }
        {
            void *callback = Dog_UpdateLeapWindup;
            u32 kind = 34;
            a->field2B2 = 1;
            a->field2B0 = zero8;
            a->field2B5 = kind;
            a->target = callback;
            a->field2C8 = zero;
            And32(&a->flags114, ~1);
            a->word104 = zero;
        }
        And32((u32 *)((u8 *)a->context + 0x678), ~2);
        {
            void *collider = context->collider;
            u32 damage = a->attacks.first[1];
            u32 response = a->attacks.second[1];
            u8 *p = (u8 *)a + 0xD0;
            u32 power = *(u32 *)p;
            u32 extra;
            p -= 10;
            extra = *(u16 *)p;
            Collider_SetAttackProperties(collider, damage, response, 0, power, extra);
        }
        context->emitted = zero8;
        context->timer698 = context->timer69C;
    }
    if (a->field2B0) {
        context->timer698 = context->timer69C;
        {
            u16 *flags = &a->flags2D4;
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
        Dog_SelectRetreatOrChase(a);
    } else
        a->counter2AC++;
}
