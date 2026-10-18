#include "global.h"
void Spider_ClearGroupFlag(u8 *);
void SceneActorPlacement_UpdateTetherTimer(u8 *);
void sub_080A3EBC(u8 *);
void SceneActorPlacement_EmitTimedParticles(u8 *);
u32 Spider_UpdatePreAction(u8 *p)
{
    Spider_ClearGroupFlag(p);
    SceneActorPlacement_UpdateTetherTimer(p);
    sub_080A3EBC(p);
    SceneActorPlacement_EmitTimedParticles(p);
    return 1;
}
