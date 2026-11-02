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

static inline void Dog_SetContextFlags(struct DogAttackContext *context, u32 mask)
{
    context->flags |= mask;
}

void Dog_UpdateTrailAttack(struct SceneActorRegistryResetView *a)
{
    struct DogAttackContext *context = a->context;
    if (Chase_TakeArrived(a)) {
        And32(&context->flags, ~8);
        SceneActorReaction_EnableMotionFlags(0x100, &a->flags2D4);
        {
            void *callback = SceneActorAnimation_EmitDirectionalTrail;
            u32 kind = 37;
            a->field2B2 = 1;
            a->field2B0 = 0;
            a->field2B5 = kind;
            a->target = callback;
            a->field2C8 = 0;
            {
                u32 *flags = &a->flags114;
                s32 mask = -2;
                *flags &= mask;
            }
            a->word104 = 0;
        }
        Dog_SetContextFlags(a->context, 2);
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
        context->emitted = 0;
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
