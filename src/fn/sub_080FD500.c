#include "global.h"
#include "scene_actor_control_phase.h"
void sub_080F8E68(void);
void ActorPhase_UpdateSpriteCollapse(void);
struct SceneActorControl35Context {
    u8 unknown0[0xE4C];
    u32 flagsE4C;
    u8 unknownE50[0x6A];
    u16 fieldEBA;
    u8 unknownEBC[6];
    u16 fieldEC2;
};
static inline void Or16(u32 m, u16 *p) { *p = m | *p; }
static inline void Or32(u32 m, u32 *p) { *p |= m; }
static inline void Restart(struct SceneActorRegistryResetView *actor)
{
        void *callback = (void *)ActorPhase_UpdateSpriteCollapse;
        u32 kind = 0x23;
        u8 zero8;
        u8 one;
        u32 zero;
        u8 *taken = &actor->field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        zero8 = 0;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
        {
            u32 phase = 2;
            u32 behavior = 5;
            actor->state2A8 = zero8;
            actor->phase2A9 = phase;
            actor->behavior2AA = behavior;
            actor->counter2AC = zero;
            actor->field2B1 = one;
        }
}
void SceneActorControl_UpdateControl35Phase(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControl35Context *context = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = (void *)sub_080F8E68;
        u32 kind = 0x35;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        Or16(0x100, &actor->flags2D4);
        Or32(0x800, &context->flagsE4C);
        context->fieldEBA = context->fieldEC2;
        Or16(0x1000, &actor->flags2C0);
    }
    if (actor->field2B0) {
        Restart(actor);
    } else {
        ++actor->counter2AC;
    }
}
