#include "player.h"
#include "player_slot_status.h"
struct Player *SceneActorQuery_FindPlayerInBounds(const struct SceneVec *center,
                                                  const struct SceneVec *radius, s32 slot)
{
    struct Player *p = gPlayers[slot];
    if (p && !PlayerSlot_ReadStatus(slot, 8) && Player_ReadStatusBits(&p->statusFlags, 0x4000) &&
        center->x - radius->x <= p->position.x && center->x + radius->x >= p->position.x &&
        center->z - radius->z <= p->position.z && center->z + radius->z >= p->position.z)
        return p;
    return 0;
}
