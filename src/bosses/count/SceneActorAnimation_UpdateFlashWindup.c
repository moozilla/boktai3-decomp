#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"

void SceneActors_StopBackgroundFlash(void *);
void BackgroundFlash_BeginAppearance(void *);

void SceneActorAnimation_UpdateFlashWindup(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 26;
        else
            sequence = 64;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 4);
        SceneActorGraphics_SetEffect(actor, 1, 826, 5);
        actor->graphics.unk3A8 = 0;
        Sound_PlaySongChecked(851);
        Sound_PlaySongChecked(887);
    }
    if (actor->graphics.stepTableKind != 3)
        SceneActorState_SetFlags(p, 0x100000);
    if (actor->graphics.unk382) {
        actor->graphics.shakeStrength = tick >> 6;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        actor->graphics.shakeAngle = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
    }
    if (actor->animation.frameIndex == 5)
        actor->animation.elapsed = 0;
    if (p->actionTryUpdate(p)) {
        SceneActorGraphics_SetEffect(actor, 2, 826, 5);
        SceneActors_StopBackgroundFlash(actor);
    } else {
        SceneActorState_SetFlags(p, 0x40);
        if (!p->actionReset(p)) {
            if (tick == 10)
                BackgroundFlash_BeginAppearance(actor->graphics.backgroundFlash);
            if (actor->animation.frameIndex == 2 && !actor->animation.elapsed)
                Sound_PlaySongChecked(849);
            if (tick > actor->graphics.flashWindupFrames)
                p->actionFinished = 1;
        }
    }
}
