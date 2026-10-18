#include "global.h"
void Spider_ClearGroupFlag(u8 *);
void sub_080A3E28(u8 *);
void sub_080A3EBC(u8 *);
void SceneActorPlacement_EmitTimedParticles(u8 *);
u32 Spider_UpdatePreAction(u8 *p)
{
    Spider_ClearGroupFlag(p);
    sub_080A3E28(p);
    sub_080A3EBC(p);
    SceneActorPlacement_EmitTimedParticles(p);
    return 1;
}
