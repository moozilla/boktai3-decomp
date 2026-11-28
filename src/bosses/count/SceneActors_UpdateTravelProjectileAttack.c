#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneSteeringActor_CreateTravelActor(void *, void *, int, int, int, int, int, int, int, int,
                                          int, int, int);

static inline void add(struct ContactPosition *a, struct ContactPosition *b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}

void SceneActors_UpdateTravelProjectileAttack(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct ContactPosition spawn;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        SceneActors_SetFacing(p, p->requestedAngle);
        if (actor->graphics.animationFlags & 1)
            sequence = 26;
        else
            sequence = 64;
        SceneActors_StartAnimation(p, &actor->animation, (int)&actor->animationResource, sequence,
                                   2, 4);
        Sound_PlaySongChecked(859);
    }
    if (!p->actionTryUpdate(p)) {
        if (!p->actionReset(p)) {
            if (tick == 16)
                Sound_PlaySongChecked(849);
            if (tick > 15 && (tick & 15) == 8) {
                SceneActor_SetChargeOffset(p->bodyDirection, &spawn, 128);
                add(&spawn, (struct ContactPosition *)&p->target);
                spawn.y += 300;
                SceneSteeringActor_CreateTravelActor(
                    &spawn, ((struct SceneGraphicsActor *)p)->playerPositions[0], 2,
                    p->requestedAngle, 90, 4, 20, 3, 25, actor->graphics.travelColliderA,
                    actor->graphics.travelColliderB, 0x40000, 30);
            }
            if (actor->animation.status & 2)
                p->actionFinished = 1;
        }
    }
}
