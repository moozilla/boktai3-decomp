#include "global.h"
#include "game_state.h"
#include "scene_marker.h"
void AnimData_SelectPaletteBank(void *, s32);
s32 PlayerBarrier_TryActivate(struct SceneMarker *marker)
{
    struct SceneMarker *active = marker;
    if (gGameState->unk77C <= 0)
        return 0;
    if (marker->colliderState != 3) {
        marker->state = 0;
        marker->colliderState = 3;
    }
    AnimData_SelectPaletteBank(&active->secondaryData, 0x65);
    Sound_PlaySong(0x198);
    return 1;
}
