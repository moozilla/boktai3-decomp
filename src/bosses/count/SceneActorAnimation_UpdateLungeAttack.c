#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"

int Math_Atan2(int, int);
static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline void decay(s16 *p)
{
    if (*p > 23)
        *p -= 24;
    else
        *p = 0;
}
void SceneActorAnimation_UpdateLungeAttack(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    u8 *position = (u8 *)p + 0x48;
    if (!p->locked)
        SceneActors_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (p->locked) {
            s16 *here = &p->target.x;
            s16 *dest;
            dest = (s16 *)((u8 *)p + 0x9c4);
            p->savedFacing = Math_Atan2(dest[0] - *(s16 *)(position + 12), dest[2] - here[2]);
            SceneActors_SetFacing(p, p->savedFacing);
        } else {
            if (actor->graphics.stepTableValuePrev == 2)
                Sound_PlaySongChecked(889);
            else
                Sound_PlaySongChecked(888);
        }
        sequence = choose(actor->graphics.animationFlags, 51, 13);
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 3, 4);
    } else {
        int sequence = choose(actor->graphics.animationFlags, 51, 13);
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 3, 1);
    }
    decay(&actor->graphics.scaleMin);
    decay(&actor->graphics.scaleMax);
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 64);
        if (!p->actionReset(p)) {
            if (tick <= 63) {
                int rate = (64 - tick) >> 3;
                struct ContactPosition *velocity = (struct ContactPosition *)(position + 20);
                u32 angle = position[9];
                int speed;
                speed = -rate;
                SceneActor_SetChargeOffset(angle, velocity, speed);
            }
            if (tick > 63)
                p->actionFinished = 1;
        }
    }
}
