#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"

void RouteSpriteActor_Create(void *, void *, int, int, int, int, int, int, int, int, int, int);

static inline void add(struct ContactPosition *a, struct ContactPosition *b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}

void SceneActors_UpdateRouteProjectileAttack(struct StateActor *p, int tick)
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
        Sound_PlaySongChecked(890);
    }
    if (!p->actionTryUpdate(p)) {
        if (!p->actionReset(p)) {
            if (tick == 16)
                Sound_PlaySongChecked(849);
            if ((tick & 31) == 16) {
                SceneActor_SetChargeOffset(p->bodyDirection, &spawn, 128);
                spawn.y = 256;
                add(&spawn, (struct ContactPosition *)&p->target);
                RouteSpriteActor_Create(&spawn,
                                        ((struct SceneGraphicsActor *)p)->playerPositions[0], 6, 6,
                                        actor->graphics.routeProjectileAmount,
                                        actor->graphics.routeProjectileKind, 0, 20, 20, 0, 0, 3);
            }
            if (actor->animation.status & 2)
                p->actionFinished = 1;
        }
    }
}
