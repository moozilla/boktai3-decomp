#include "global.h"
#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_chase_shared.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
struct ActorTrail {
    u8 unknown0[0x8D6];
    s16 timer;
    u8 unknown8D8[0x1C];
    u16 soundDelay;
};
void ActorTrail_UpdateTimedScale(struct ChaseActor *actor)
{
    struct ActorTrail *trail;
    if (SceneActor_ActionsBlocked(actor))
        return;
    trail = actor->base.context;
    if (trail->timer > 0) {
        if (trail->timer <= 59)
            actor->base.field2C2 = 32;
        else {
            if (*(s16 *)&actor->base.heightOffset == 0 && actor->base.secondaryMotion.y == 0) {
                if (trail->soundDelay)
                    trail->soundDelay--;
                else if (Mod(trail->timer, 24) == 0)
                    Sound_PlaySong(984);
            }
            actor->base.field2C2 = Div(64, 3);
        }
    } else
        actor->base.field2C2 = 64;
}
