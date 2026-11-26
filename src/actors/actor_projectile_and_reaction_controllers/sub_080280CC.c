#include "actors/common_scene_actor_controller/scene_actors.h"
#include "engine/shared/scene_graphics_actor.h"
#include "engine/shared/scene_actor_charge_offset.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/ram.h"

static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline void add(struct ContactPosition *a, struct ContactPosition *b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}
void SceneActors_UpdateSwarmAttack(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct StateActor *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    struct ContactPosition spawn;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        sequence = choose(actor->graphics.animationFlags, 68, 30);
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
        if (p->animationFacing4 == 0 || p->animationFacing4 == 3)
            SceneActor_SetChargeOffset((p->animationFacing4 * 64) - 64, &spawn, 180);
        else
            SceneActor_SetChargeOffset((p->animationFacing4 * 64) - 64, &spawn, 110);
        add(&spawn, (struct ContactPosition *)(position + 12));
        spawn.y += 300;
        direction = ((struct SceneGraphicsActor *)p2)->graphics.projectileDirection;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 random = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
            direction -= 8;
            random &= 15;
            direction += random;
        }
        ContactSwarm_SpawnCharge(*(void **)((u8 *)p2 + 0x9d4), &spawn, direction, 20, 0, 0, 0);
        Sound_PlaySongChecked(234);
    }
    if (((struct SceneGraphicsActor *)p2)->animation.status & 2) {
        if (tick > 40)
            p->actionFinished = 1;
    }
}
