#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"
#include "engine/video/render.h"

int SceneActorGraphics_GetResourceId(void *);
void TemporaryAnimationPool_CreateEntry(void *, int, int, int, void *);

void SceneActorAnimation_UpdateChargeSlide(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 15;
        else
            sequence = 53;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 3, 4);
    }
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 0x40);
        if (!p->actionReset(p)) {
            if (tick <= 19)
                SceneActor_SetChargeOffset(body->direction, &body->motion, 20 - tick);
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
            if (tick > 59)
                p->actionFinished = 1;
        }
    }
}
