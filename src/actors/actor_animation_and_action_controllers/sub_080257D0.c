#include "actors/common_scene_actor_controller/scene_actors.h"
#include "engine/shared/scene_actor_charge_offset.h"
#include "engine/shared/scene_graphics_actor.h"
#include "engine/shared/render.h"

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
void SceneActorAnimation_UpdateChargeSlide(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    u8 *position = (u8 *)p + 0x48;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        sequence = choose(actor->graphics.animationFlags, 53, 15);
        {

            SceneActors_StartDirectionalAnimation(
                p, &actor->animation, (int)&actor->animationResource, sequence, 2, 3, 4);
        }
    }
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 64);
        if (!p->actionReset(p)) {
            if (tick <= 19) {
                struct ContactPosition *velocity = (struct ContactPosition *)(position + 20);
                u32 angle = position[9];
                int speed;
                speed = 20 - tick;
                SceneActor_SetChargeOffset(angle, velocity, speed);
            }
            if (!(tick & 3)) {
                if (!actor->graphics.particleFlag) {
                    int id = SceneActorGraphics_GetResourceId(actor);
                    u8 *data = gPaletteBankData + id * 32;
                    TemporaryAnimationPool_CreateEntry(&actor->animation, 8, 0x200, 0x9000, data);
                } else {
                    u8 *data;
                    data = gPaletteBankData;
                    data += 0x6740;
                    {

                        TemporaryAnimationPool_CreateEntry(&actor->animation, 8, 0x200, 0x9000,
                                                           data);
                    }
                }
            }
            if (tick > 59)
                p->actionFinished = 1;
        }
    }
}
