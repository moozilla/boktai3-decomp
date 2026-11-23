#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_chase_shared.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
struct ActorTrail {
    u8 unknown0[0x8D6];
    s16 timer;
    u8 unknown8D8[0x1C];
    u16 soundDelay;
};
static inline u32 stateFlag(struct ChaseActor *p, u32 mask)
{
    return p->base.motionFlags & mask;
}
static inline u8 unavailable(struct ChaseActor *p)
{
    if (p && (stateFlag(p, 0x40) || SceneActorChase_StatusFlags(p, 0x20000) || p->base.health <= 0))
        return 1;
    return 0;
}
void ActorTrail_UpdateTimedScale(struct ChaseActor *actor)
{
    struct ActorTrail *trail;
    if (unavailable(actor))
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
