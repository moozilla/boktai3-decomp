#include "global.h"
struct SceneNpc {
    u8 unknown0[5];
    u8 commandPhase;
};
s32 Actor_UpdateMovementAnimationAndEffects(void *, struct SceneNpc *);
void SceneNpc_DispatchCommands_081CD484(struct SceneNpc *);
s32 SceneNpc_UpdateCommands(void *context, struct SceneNpc *p)
{
    Actor_UpdateMovementAnimationAndEffects(context, p);
    SceneNpc_DispatchCommands_081CD484(p);
    return p->commandPhase = 0;
}
