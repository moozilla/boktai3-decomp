#include "global.h"
#include "scene_actor_control_motion.h"
#include "display_sequence_inline.h"

struct GolemCombatResetContext {
    u8 pad0[0x624];
    u8 b624;
    u8 pad625[3];
    u32 w628;
    u8 pad62c[0x50];
    u32 w67c;
    u16 h680, h682, h684, h686, h688, h68a;
    u8 b68c, b68d, b68e, b68f, b690, b691;
    u8 pad692[2];
    u8 a694[8];
    u8 a69c[8];
    u8 a6a4[8];
    u16 h6ac, h6ae;
};
struct GolemResetActor {
    u8 pad0[4];
    void *spr;
};

void Memory_Clear(void *, int);
void SceneActorMovement_ResolveRoutePosition(void *);

// confidence=low: resets golem combat counters and restarts the idle animation.
void Golem_ResetCombatState(u8 *p)
{
    struct GolemCombatResetContext *in = *(struct GolemCombatResetContext **)(p + 0x3d0);
    in->b624 = 0;
    in->w628 = 0;
    in->w67c = 0;
    in->h682 = 0;
    in->h680 = 0;
    in->h684 = 0;
    in->h686 = 0;
    in->h688 = 0;
    in->h68a = 0;
    in->b68c = 0;
    in->b68d = 0;
    in->b68e = 0;
    in->b68f = 0;
    in->b690 = 0;
    in->b691 = 0;
    Memory_Clear(in->a694, 8);
    Memory_Clear(in->a69c, 8);
    Memory_Clear(in->a6a4, 8);
    Memory_Clear((u8 *)in + 0x674, 8);
    in->h6ac = 0;
    in->h6ae = 0;
    SceneActorMovement_ResolveRoutePosition(p);
    SceneActorAnimation_SelectDirectionalEntry((void *)p, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = ((struct GolemResetActor *)p)->spr;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}
