#include "scene_actor_control.h"
#include "game_state.h"
struct SceneActorControlSystem {
    u8 unknown0[14];
    u8 mode;
};
extern struct SceneActorControlSystem gSunClock;
static inline u8 MatchesScene(s32 scene, struct GameState *state)
{
    if (state->region == scene)
        return 1;
    return 0;
}
u32 SceneActorControl_IsEnabledForScene(struct SceneActorControlSlot *slot)
{
    u32 mode = gSunClock.mode;
    if (mode != 5 && mode != 0 && mode != 4 && (gGameState->flags868 & 2) &&
        MatchesScene(slot->scene, gGameState))
        return 1;
    return 0;
}
