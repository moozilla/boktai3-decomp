#include "global.h"
#include "player/effects/scene_marker.h"
void AnimData_SelectPaletteBank(void *, s32);
void PlayerBarrier_BeginRecovery(struct SceneMarker *marker)
{
    if (marker != 0) {
        marker->colliderState = 2;
        AnimData_SelectPaletteBank(&marker->secondaryData, 0x65);
    }
}
