#include "interaction_marker.h"
u32 InteractionMarker_IsPlayerNearby(struct InteractionMarker *marker)
{
    u16 state = (u16)gGameState->form - 3;
    struct InteractionMarkerPlayer *player;
    s32 x, z;
    if (state <= 2)
        return 0;
    player = gPlayers[0];
    if (player->busy)
        return 0;
    x = player->position.x - marker->position.x;
    if (x < 0)
        x = -x;
    z = player->position.z - marker->position.z;
    if (z < 0)
        z = -z;
    if (x > 96 || z > 96)
        return 0;
    if (x * x + z * z > 9216)
        return 0;
    return 1;
}
