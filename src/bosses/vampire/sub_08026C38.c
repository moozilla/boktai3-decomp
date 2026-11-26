#include "scene/actor/scene_actors.h"
#include "scene/actor/scene_actor_charge_offset.h"
#include "bosses/vampire/scene_graphics_actor.h"

static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
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
    struct StateActor *p2 = p;
    struct ContactPosition spawn;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        SceneActors_SetFacing(p, p->requestedAngle);
        sequence = choose(actor->graphics.animationFlags, 64, 26);
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
                add(&spawn, (struct ContactPosition *)((u8 *)p + 0x54));
                spawn.y += 300;
                SceneSteeringActor_CreateTravelActor(
                    &spawn, *(void **)((u8 *)p + 0x110), 2, p->requestedAngle, 90, 4, 20, 3, 25,
                    ((struct SceneGraphicsActor *)p2)->graphics.travelColliderA,
                    ((struct SceneGraphicsActor *)p2)->graphics.travelColliderB, 0x40000, 30);
            }
            if (((struct SceneGraphicsActor *)p2)->animation.status & 2)
                p->actionFinished = 1;
        }
    }
}
