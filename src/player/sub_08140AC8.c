#include "engine/shared/contact_position.h"
#include "engine/shared/player.h"

/* tentative: probes tiles beside the facing and picks a diagonal direction */

union TerrainProbePosition {
    struct SceneVec position;
    struct ContactPosition contact;
};

s32 Player_IsTurnTerrainBlocked(struct Player *player, const struct ContactPosition *position);

s32 Player_SelectLockedTurnDirection(struct Player *player)
{
    union TerrainProbePosition probe;

    switch (player->unk48D[2]) {
        case 0:
            probe.position = player->position;
            probe.position.x -= 30;
            probe.position.z -= 30;
            probe.position.x -= 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 1;
            probe.position.x += 100;
            probe.position.z -= 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 7;
            probe.position = player->position;
            probe.position.x -= 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 1;
            return 7;
        case 2:
            probe.position = player->position;
            probe.position.x += 30;
            probe.position.z -= 30;
            probe.position.x += 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 1;
            probe.position.x -= 100;
            probe.position.z -= 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 3;
            probe.position = player->position;
            probe.position.x += 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 1;
            return 3;
        case 4:
            probe.position = player->position;
            probe.position.x += 30;
            probe.position.z += 30;
            probe.position.x += 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 5;
            probe.position.x -= 100;
            probe.position.z += 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 3;
            probe.position = player->position;
            probe.position.x += 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 5;
            return 3;
        case 6:
            probe.position = player->position;
            probe.position.x -= 30;
            probe.position.z += 30;
            probe.position.x -= 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 5;
            probe.position.x += 100;
            probe.position.z += 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 7;
            probe.position = player->position;
            probe.position.x -= 100;
            if (Player_IsTurnTerrainBlocked(player, &probe.contact))
                return 5;
            return 7;
    }
    return -1;
}
