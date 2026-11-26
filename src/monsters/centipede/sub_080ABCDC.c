#include "global.h"

struct TentacleBody;
u32 Centipede_UpdatePendingVelocity(struct TentacleBody *);
void Callback_InvokeActor(u8 *, u32);
void SceneActorControl_DispatchControl(u8 *);
void Centipede_UpdateComponents(u8 *);

u32 Centipede_OnUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    Centipede_UpdatePendingVelocity((struct TentacleBody *)p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    Centipede_UpdateComponents(p);
    return 1;
}
