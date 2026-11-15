#include "resource_station.h"
void Player_BeginResourceStationInteraction(struct ResourceStationPlayer *,
                                            struct ResourceStationPosition *, u32);
static inline u32 ResourceStation_IsBlocked(void)
{
    u32 state = gPlayerPassageState;
    u32 blocked = 0;
    if (state && gPlayerInteractionMode == 5)
        blocked = 1;
    return blocked;
}
void ResourceStation_CheckInteraction(struct ResourceStation *station)
{
    struct ResourceStationPlayer *player = station->player;
    s32 x, z;
    u32 kind;
    if (player->position.y == station->triggerPosition.y) {
        x = player->position.x - station->triggerPosition.x;
        if (x < 0)
            x = -x;
        z = player->position.z - station->triggerPosition.z;
        if (z < 0)
            z = -z;
        if (x > 128 || z > 128) {
            station->nearby = 0;
            return;
        }
        if (player->stateTag != 1) {
            station->nearby = 0;
            return;
        }
        station->nearby = 1;
        if (!ResourceStation_IsBlocked()) {
            player = station->player;
            kind = player->actionMode;
            if (kind == 3)
                return;
            if (kind == 4)
                return;
            if (kind == 5)
                return;
            if (player->busy)
                return;
            if (!(player->inputKeys[1] & 1))
                return;
            if (player->action != 7)
                return;
            Player_BeginResourceStationInteraction(player, &station->triggerPosition,
                                                   station->interactionVariant);
            station->state = 1;
        }
        return;
    }
    station->nearby = 0;
}
