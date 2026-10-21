#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.8%
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
s32 Mod(s32, s32);
#define FIELD(p, o, t) (*(t *)((u8 *)(p) + (o)))
static inline u16 Flag(struct SceneActorRegistryResetView *actor, u32 input)
{
    register u32 mask asm("r1") = input;
    asm("" : "+r"(mask));
    {
        register u32 result asm("r0") = actor->flags2D4 & mask;
        asm("" : "+r"(result));
        return result;
    }
}
static inline void State(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                         u32 behavior, u32 counter)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = counter;
    actor->field2B1 = 1;
}
// confidence=low: retains and halves stored horizontal velocity until the animation finishes.
void Ghost_DecelerateMovement(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    u32 zero;
    s16 *vx, *vz;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = FIELD(actor, 0x224, void *);
        u32 kind = 2;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        {
            register u32 offset asm("r4") = 0x2C8;
            register u16 *output asm("r0");
            asm("" : : "r"(offset));
            output = (u16 *)((u8 *)actor + offset);
            *output = 0;
        }
        {
            register u32 offset asm("r0") = 0x114;
            register u32 *flags asm("r2");
            register u32 mask asm("r1");
            asm("" : : "r"(offset));
            flags = (u32 *)((u8 *)actor + offset);
            mask = ~1;
            *flags &= mask;
        }
        {
            register u32 offset asm("r1") = 0x104;
            register u32 *output asm("r0");
            asm("" : : "r"(offset));
            output = (u32 *)((u8 *)actor + offset);
            *output = 0;
        }
    }
    {
        register u32 off asm("r2") = 0x5FC;
        asm("" : "+r"(off));
        vx = (s16 *)(context + off);
        actor->motion.x += *(u16 *)vx;
    }
    zero = 0;
    actor->motion.z += FIELD(context, 0x600, u16);
    vz = (s16 *)(context + 0x600);
    if (Mod(actor->counter2AC, 4) == 0) {
        *vx >>= 1;
        *vz >>= 1;
    }
    if (actor->field2B0) {
        u32 flag = Flag(actor, 0x100);
        if (flag) {
            State(actor, zero, 2, 1, 0);
        } else {
            State(actor, zero, zero, zero, flag);
        }
    } else
        ++actor->counter2AC;
}

#else
INCLUDE_ASM("asm/nonmatching", Ghost_DecelerateMovement);
#endif
