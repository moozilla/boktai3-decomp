#define SceneActorControl_UpdateAnimationTransition sub_080EE000
#define SceneActorControl_UpdateDirectionalIdle     sub_080ED3A0
#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "display_animation.h"

void sub_080ED3A0(u8 *);
static inline void Set16(u16 *p, u32 m)
{
    *p = m | *p;
}
static inline void Clear16(u16 *p, u32 m)
{
    *p = m & *p;
}
static inline void Clear32(u32 *p, u32 m)
{
    *p = m & *p;
}
void sub_080EE000(struct SceneActorRegistryResetView *actor, s32 elapsed)
{
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->drawable + 0x48);
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 3);
        actor->field2B3 = 0;
        actor->colliderA.flags = (actor->colliderA.flags | 4) & 0xFFF7;
        Set16(&actor->flags2C0, 0x800);
        Set16(&actor->flags2D4, 0x20);
    }
    Set16(&actor->flags2D6, 0x10);
    if (elapsed <= 0x11 && sequence->index == 0 && sequence->elapsed == !!sequence->duration &&
        sequence->duration != 1)
        sequence->elapsed = 0;
    if (actor->field2B3) {
        actor->field2B0 = 0;
        Clear16(&actor->flags2D4, ~0x20);
        Clear32(&actor->flags2CC, ~0x80);
        SceneActorReaction_SetControl(actor, 0x22, sub_080ED3A0);
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        actor->direction = actor->fieldE9;
    }
}
