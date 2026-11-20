#include "scene_actors.h"
#include "scene_actor_charge_offset.h"
#include "scene_graphics_actor.h"
#include "render.h"

static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
int SceneActorGraphics_GetResourceId(void *);
void TemporaryAnimationPool_CreateEntry(void *, int, int, int, void *);
typedef struct {
    u32 a, b;
} Point;
static inline void setFlags(struct StateActor *p, u32 flags)
{
    u32 mask = flags;
    p->actionFlags |= mask;
}
void SceneActorAnimation_UpdateHomingLunge(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    u8 *position = (u8 *)p + 0x48;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        sequence = choose(actor->graphics.animationFlags, 52, 14);
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 3, 4);
        Sound_PlaySongChecked(842);
    }
    if (actor->graphics.particleFlag == 1)
        setFlags(p, 8);
    else
        setFlags(p, 0x40000);
    if (p->actionTryUpdate(p))
        return;
    setFlags(p, 0x40);
    if (p->actionReset(p))
        return;
    if (!(tick & 3)) {
        if (!actor->graphics.particleFlag) {
            int id = SceneActorGraphics_GetResourceId(actor);
            u8 *data = gPaletteBankData + id * 32;
            TemporaryAnimationPool_CreateEntry(&actor->animation, 8, 0x200, 0x9000, data);
        } else {
            u8 *data = gPaletteBankData + 0x6740;
            TemporaryAnimationPool_CreateEntry(&actor->animation, 8, 0x200, 0x9000, data);
        }
    }
    if (p->locked) {
        s16 *dest = (s16 *)((u8 *)actor + 0x9c4);
        int dx = dest[0] - *(s16 *)(position + 12), dz = dest[2] - *(s16 *)(position + 16);
        if (dx * dx + dz * dz < 1600) {
            *(Point *)(position + 12) = *(Point *)dest;
            goto finished;
        }
    }
    {
        struct ContactPosition *velocity = (struct ContactPosition *)(position + 20);
        u32 angle = position[9];
        int speed;
        speed = 40;
        SceneActor_SetChargeOffset(angle, velocity, speed);
    }
    if (tick > 159) {
    finished:
        p->actionFinished = 1;
    }
}
