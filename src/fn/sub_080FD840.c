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
    struct SceneActorExtendedControlContext *context = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        context->fieldED1 = 1;
    if (actor->field2B0) {
        u8 zero8;
        u32 zero;
        Axe_SetAuxiliarySpriteRenderState(actor, context->target888, 0x13C);
        actor->flags2D4 = Mask16(&actor->flags2D4, ~0x100);
        zero8 = 0;
        zero = 0;
        {
            u32 *flags = &context->flagsE4C;
            u32 mask = ~0x1000;
            *flags &= mask;
        }
        {
            u32 behavior = 3;
            actor->state2A8 = zero8;
            actor->phase2A9 = zero8;
            actor->behavior2AA = behavior;
            actor->counter2AC = zero;
            actor->field2B1 = 1;
        }
    } else {
        ++actor->counter2AC;
    }
}
