#include "engine/shared/global.h"
#include "engine/shared/scene_actor_control_motion.h"
#include "player/player_cue/player_cue_sequence.h"
#include "engine/shared/scene_actor_terrain.h"
void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);
static inline u32 StepDrawable(struct SceneActorControlMotionDrawable *owner)
{
    return PlayerCue_StepSequenceEnded(owner, &owner->sequence);
}
void SceneActorControl_RestoreMotion(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
    u32 height;
    state->field3DB = 0;
    state->field3E0 = 0;
    state->phase = 0;
    state->swayStep = 1;
    state->sway = 0;
    state->elapsed = 0;
    state->startTick = 0;
    state->flags3D4 = 0;
    actor->base.body.position = state->storedPosition;
    actor->base.terrainCell = SceneActorTerrain_GetCellIndex(&state->storedPosition);
    height = SceneActorTerrain_GetHeight(actor->base.terrainCell, &state->storedPosition);
    actor->base.groundHeight = height;
    actor->base.heightOffset = actor->base.body.position.y - height;
    actor->base.body.position.y = actor->base.groundHeight;
    if (actor->base.drawableKind == 0)
        ((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->position =
            actor->base.body.position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->position =
            actor->base.body.position;
    ZeroFill8(&state->vectors[0]);
    ZeroFill8(&state->vectors[1]);
    ZeroFill8(&state->vectors[2]);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    StepDrawable(drawable);
}
