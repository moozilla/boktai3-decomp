#define SceneActorControl_UpdateControl2APhase sub_080FD8FC
#include "global.h"
#include "scene_actor_control_phase.h"
void SceneActor_AdvanceOffsetApproach(void);
void sub_080F82E8(void);
struct SceneActorControl2AContext {
    u8 unknown0[0xEBE];
    u16 fieldEBE;
    u8 unknownEC0[6];
    u16 fieldEC6;
};
static inline void Or16(u32 m, u16 *p) { *p = m | *p; }
static inline void Or32(u32 m, u32 *p) { *p |= m; }
static inline u16 Mask16(u16 *p, u32 m) { return *p & m; }
static inline void Restart(struct SceneActorRegistryResetView *actor)
{
    u8 zero8;
    u32 zero;
    actor->flags2D4 = Mask16(&actor->flags2D4, ~0x100);
    zero8 = 0;
    zero = 0;
    {
        void *callback = (void *)sub_080F82E8;
        u32 kind = 0x23;
        u8 one;
        u8 *taken = &actor->field2B2;
        one = 1;
        *taken = one;
        actor->field2B0 = zero8;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
        {
            u32 behavior = 4;
            actor->state2A8 = zero8;
            actor->phase2A9 = zero8;
            actor->behavior2AA = behavior;
            actor->counter2AC = zero;
            actor->field2B1 = one;
        }
    }
}
void sub_080FD8FC(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControl2AContext *context = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = (void *)SceneActor_AdvanceOffsetApproach;
        u32 kind = 0x2A;
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
        context->fieldEBE = context->fieldEC6;
    }
    if (actor->field2B0) {
        Restart(actor);
    } else {
        ++actor->counter2AC;
    }
}
