#include "global.h"
#include "engine/video/proximity_volume.h"
#include "player/player.h"

s32 Nav_IsPositionInMesh(void *, void *, void *);
u32 NavMesh_FindRoute(void *, void *, void *);
u32 ProximityVolume_ContainsPlayer(struct ProximityVolume *volume)
{
    struct SceneVec *player = &gPlayers[0]->position;

    if (Nav_IsPositionInMesh(volume->probe, &volume->position, player) &&
        NavMesh_FindRoute(volume->probe, &volume->position, player))
        return 1;
    return 0;
}
