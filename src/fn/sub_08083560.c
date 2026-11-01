#include "global.h"
#include "scene_actor_control_motion.h"
#include "player_cue_sequence.h"
#include "scene_actor_terrain.h"
void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorAnimationView *, s32);
static inline u32 StepDrawable(struct SceneActorControlMotionDrawable *owner)
{
    return PlayerCue_StepSequenceEnded(owner, &owner->sequence);
}
void SceneActorControl_RestoreMotion(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    struct SceneActorControlMotionDrawable *drawable = actor->drawable;
    u32 height;
    state->field3DB = 0;
    state->field3E0 = 0;
    state->phase = 0;
    state->swayStep = 1;
    state->sway = 0;
    state->elapsed = 0;
    state->startTick = 0;
    state->flags3D4 = 0;
    actor->body.position = state->storedPosition;
    actor->terrainCell = SceneActorTerrain_GetCellIndex(&state->storedPosition);
    height = SceneActorTerrain_GetHeight(actor->terrainCell, &state->storedPosition);
    actor->groundHeight2BE = height;
    actor->field2EA = actor->body.position.y - height;
    actor->body.position.y = actor->groundHeight2BE;
    if (actor->drawableKind == 0)
        ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position =
            actor->body.position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->position = actor->body.position;
    ZeroFill8(&state->vectors[0]);
    ZeroFill8(&state->vectors[1]);
    ZeroFill8(&state->vectors[2]);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    StepDrawable(drawable);
}
