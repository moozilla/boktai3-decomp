#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"

static inline void ContactPosition_Add(struct ContactPosition *dst, const struct ContactPosition *src)
{
    dst->x += src->x;
    dst->y += src->y;
    dst->z += src->z;
}

void SceneActors_UpdateSwarmAttack(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = SceneActors_Body(p);
    struct ContactPosition spawn;

    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 30;
        else
            sequence = 68;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 4);
        Sound_PlaySongChecked(860);
    }
    if (p->actionTryUpdate(p))
        return;
    if (p->actionReset(p))
        return;
    if (tick > 10 && (tick & 7) == 7) {
        int direction;
        u32 random;
        if (p->animationFacing4 == 0 || p->animationFacing4 == 3)
            SceneActor_SetChargeOffset(p->animationFacing4 * 64 - 64, &spawn, 180);
        else
            SceneActor_SetChargeOffset(p->animationFacing4 * 64 - 64, &spawn, 110);
        ContactPosition_Add(&spawn, &body->position);
        spawn.y += 300;
        direction = actor->graphics.projectileDirection;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        random = ((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
        direction -= 8;
        direction += random & 15;
        CountBats_SpawnCharge(*(struct ContactSwarm **)((u8 *)actor + 0x9d4), &spawn, direction, 20, 0, 0, 0);
        Sound_PlaySongChecked(234);
    }
    if (actor->animation.status & 2) {
        if (tick > 40)
            p->actionFinished = 1;
    }
}
