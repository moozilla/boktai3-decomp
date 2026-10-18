#include "global.h"
#define SceneNpc_UpdateCommands sub_081CDFFC
#define SceneNpc_DispatchCommands sub_081CD484
struct SceneNpc { u8 unknown0[5]; u8 commandPhase; };
s32 Actor_UpdateMovementAnimationAndEffects(void *, struct SceneNpc *);
void sub_081CD484(struct SceneNpc *);
s32 sub_081CDFFC(void *context, struct SceneNpc *p)
{
    Actor_UpdateMovementAnimationAndEffects(context, p);
    sub_081CD484(p);
    return p->commandPhase = 0;
}
