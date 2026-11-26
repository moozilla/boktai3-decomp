#include "monsters/centipede/centipede_body.h"

u32 Centipede_UpdatePendingVelocity(struct TentacleBody *);
void SceneActorControl_DispatchControl(u8 *);
void Centipede_UpdateComponents(u8 *);

u32 Centipede_OnUpdate(u8 *p)
{
    struct TentacleBody *body = (void *)p;
    u32 i;
    void **callbacks;
    Centipede_UpdatePendingVelocity(body);
    i = body->base.state2A8;
    callbacks = body->base.stateCallbacks;
    Callback_InvokeActor(body, callbacks[i]);
    SceneActorControl_DispatchControl(p);
    Centipede_UpdateComponents(p);
    return 1;
}
