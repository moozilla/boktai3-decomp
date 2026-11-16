#define SceneActorControl_SetFourChannelTargets Axe_SetAuxiliarySpriteRenderState
#include "global.h"
#include "scene_actor_control_phase.h"
struct SceneActorExtendedControlContext {
    u8 unknown0[0x888];
    u8 target888[0x5C4];
    u32 flagsE4C;
    u8 unknownE50[0x81];
    u8 fieldED1;
};
void Axe_SetAuxiliarySpriteRenderState(struct SceneActorRegistryResetView *, void *, u32);
static inline u16 Mask16(u16 *p, u32 m)
{
    return *p & m;
}
void SceneActorControl_EndExtendedPhase(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorExtendedControlContext *context = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        context->fieldED1 = 1;
    if (actor->base.field2B0) {
        u8 zero8;
        u32 zero;
        Axe_SetAuxiliarySpriteRenderState(actor, context->target888, 0x13C);
        actor->base.flags2D4 = Mask16(&actor->base.flags2D4, ~0x100);
        zero8 = 0;
        zero = 0;
        {
            u32 *flags = &context->flagsE4C;
            u32 mask = ~0x1000;
            *flags &= mask;
        }
        {
            u32 behavior = 3;
            actor->base.state2A8 = zero8;
            actor->base.phase2A9 = zero8;
            actor->base.behavior2AA = behavior;
            actor->base.counter2AC = zero;
            actor->base.field2B1 = 1;
        }
    } else {
        ++actor->base.counter2AC;
    }
}
