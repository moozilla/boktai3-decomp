#include "dog.h"
#include "scene_actor_chase.h"
#define FIELD(p, n, T) (*(T *)((u8 *)(p) + (n)))
void SceneActorAnimation_EmitDirectionalTrail(void);
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

static inline void Dog_SetContextFlags(struct Dog *context, u32 mask)
{
    context->flags |= mask;
}

void Dog_UpdateTrailAttack(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    if (Chase_TakeArrived(a)) {
        And32(&context->flags, ~8);
        SceneActorReaction_EnableMotionFlags(0x100, &a->base.flags2D4);
        {
            void *callback = SceneActorAnimation_EmitDirectionalTrail;
            u32 kind = 37;
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
        Dog_SetContextFlags(a->base.context, 2);
        {
            void *collider = &context->collider;
            u32 damage = a->base.attacks.first[2];
            u32 response = a->base.attacks.second[2];
            u8 *p = (u8 *)a + 0xD4;
            u32 power = *(u32 *)p;
            u32 extra;
            p -= 12;
            extra = *(u16 *)p;
            Collider_SetAttackProperties(collider, damage, response, 0, power, extra);
        }
        context->emitted = 0;
    }
    if (a->base.field2B0) {
        context->farTimer = context->farDistance;
        {
            u16 *flags = &a->base.flags2D4;
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
        Dog_SelectRetreatOrChase(a);
    } else
        a->base.counter2AC++;
}
