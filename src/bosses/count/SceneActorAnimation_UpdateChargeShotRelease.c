#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"

int Math_Atan2(int, int);
static inline void add(struct ContactPosition *a, struct ContactPosition *b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}

static inline int clampTurn(int delta)
{
    if (delta > 32)
        delta = 32;
    else if (delta < -32)
        delta = -32;
    return delta;
}

void SceneActorAnimation_UpdateChargeShotRelease(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;
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
        int power;
        int direction;
        if (p->animationFacing4 == 0 || p->animationFacing4 == 3)
            SceneActor_SetChargeOffset((p->animationFacing4 * 64) - 64, &spawn, 180);
        else
            SceneActor_SetChargeOffset((p->animationFacing4 * 64) - 64, &spawn, 110);
        add(&spawn, &body->position);
        spawn.y += 300;
        power = actor->graphics.chargeShotPower;
        direction = body->direction + clampTurn(p->requestedAngle - body->direction);
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 random = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
            direction -= 8;
            random &= 15;
            direction += random;
        }
        CountBats_SpawnCharge(actor->graphics.batPool, &spawn, direction, 20, power, 30, 8);
        Sound_PlaySongChecked(234);
    }
    if (actor->animation.status & 2) {
        if (tick > 40)
            p->actionFinished = 1;
    }
}
