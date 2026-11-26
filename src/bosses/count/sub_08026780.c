#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline void effect(u8 *p, int mode, int id, int duration)
{
    *(u32 *)(p + 0x390) = id;
    p[0x38e] = mode;
    p[0x38f] = duration;
    if (p[0x38e] == 2) {
        u32 value = duration == 5 ? 32 : 64;
        p[0x38c] = value;
    } else if (p[0x38e] == 1)
        p[0x38c] = 0;
}
void BackgroundFlash_BeginAlternating(void *);
void SceneActorAnimation_UpdateFlashAlternating(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        int sequence;
        struct AnimSprite *draw;
        int resource;
        p->actionChanged = 0;
        sequence = choose(actor->graphics.animationFlags, 66, 28);
        draw = &actor->animation;
        resource = (int)&actor->animationResource;
        SceneActors_StartDirectionalAnimation(p, draw, resource, sequence, 2, 1, 0);
        actor->animation.frameIndex = 2;
        SceneActors_UpdateAnimation(p, draw, resource);
        BackgroundFlash_BeginAlternating(actor->graphics.backgroundFlash);
        if (actor->graphics.stepTableKind == 1)
            Sound_PlaySongChecked(900);
        else
            Sound_PlaySongChecked(850);
        Sound_PlaySongChecked(886);
    }
    if (p->actionTryUpdate(p)) {
        if (tick <= 32)
            effect((u8 *)actor, 2, 826, 5);
    } else {
        SceneActorState_SetFlags(p, 64);
        if (!p->actionReset(p)) {
            if (tick == 32)
                effect((u8 *)actor, 2, 826, 5);
            if (actor->animation.status & 2)
                p->actionFinished = 1;
        }
    }
}
