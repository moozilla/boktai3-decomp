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
            u8 *ctx = a->context;
            u32 bit = 2;
            u32 *p;
            p = (void *)(ctx + 0x678);
            *p |= bit;
        }
        {
            void *collider = context->collider;
            u32 damage = a->attacks.first[2];
            u32 response = a->attacks.second[2];
            u8 *p = (u8 *)a + 0xD4;
            u32 power = *(u32 *)p;
            u32 extra;
            p -= 12;
            extra = *(u16 *)p;
            Collider_SetAttackProperties(collider, damage, response, 0, power, extra);
        }
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
