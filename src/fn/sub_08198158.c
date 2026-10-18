#include "global.h"
#include "game_state.h"
#define SceneActor_SetSavedPositionOffset sub_08198158
#define gSavedGameState gUnk_02000710
struct OffsetSceneActor {
    u8 unknown0[0x30];
    u32 flags;
    u8 unknown34[0x14];
    s16 targetX, targetY, targetZ;
};
void sub_0819804C(struct OffsetSceneActor *);
void sub_08198158(struct OffsetSceneActor *actor)
{
    sub_0819804C(actor);
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
