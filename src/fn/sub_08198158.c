#include "global.h"
#include "game_state.h"
#define gSavedGameState gGameState
struct OffsetSceneActor {
    u8 unknown0[0x30];
    u32 flags;
    u8 unknown34[0x14];
    s16 targetX, targetY, targetZ;
};
void ShadowActor_UpdateTransition(struct OffsetSceneActor *);
void SceneActor_SetSavedPositionOffset(struct OffsetSceneActor *actor)
{
    ShadowActor_UpdateTransition(actor);
    if (actor->flags & 4) {
        actor->targetX = gSavedGameState->positionX + 0x120;
        actor->targetY = gSavedGameState->positionY;
        actor->targetZ = gSavedGameState->positionZ - 0x120;
    } else {
        actor->targetX = gSavedGameState->positionX - 0x120;
        actor->targetY = gSavedGameState->positionY;
        actor->targetZ = gSavedGameState->positionZ + 0x120;
    }
}
