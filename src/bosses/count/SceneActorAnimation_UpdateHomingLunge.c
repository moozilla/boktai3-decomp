#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"
#include "engine/video/render.h"

int SceneActorGraphics_GetResourceId(void *);
void TemporaryAnimationPool_CreateEntry(void *, int, int, int, void *);

void SceneActorAnimation_UpdateHomingLunge(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 14;
        else
            sequence = 52;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 3, 4);
        Sound_PlaySongChecked(842);
    }
    if (actor->graphics.particleFlag == 1)
        SceneActorState_SetFlags(p, 8);
    else
        SceneActorState_SetFlags(p, 0x40000);
    if (p->actionTryUpdate(p))
        return;
    SceneActorState_SetFlags(p, 0x40);
    if (p->actionReset(p))
        return;
    if (!(tick & 3)) {
        if (!actor->graphics.particleFlag) {
            int id = SceneActorGraphics_GetResourceId(actor);
            u8 *data = gPaletteBankData + id * 32;
            TemporaryAnimationPool_CreateEntry(&actor->animation, 8, 0x200, 0x9000, data);
        } else {
            u8 *data = gPaletteBankData + 826 * 32;
            TemporaryAnimationPool_CreateEntry(&actor->animation, 8, 0x200, 0x9000, data);
        }
    }
    if (p->locked) {
        struct ContactPosition *snap = &actor->graphics.snapPosition;
        int dx = snap->x - body->position.x, dz = snap->z - body->position.z;
        if (dx * dx + dz * dz < 40 * 40) {
            body->position = *snap;
            p->actionFinished = 1;
            return;
        }
    }
    SceneActor_SetChargeOffset(body->direction, &body->motion, 40);
    if (tick > 159)
        p->actionFinished = 1;
}
