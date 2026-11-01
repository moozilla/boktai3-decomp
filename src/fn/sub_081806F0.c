#include "global.h"
#include "contact_actor.h"
#include "player.h"
struct ProximityVolume {
    u8 unknown0[8];
    struct ContactPosition position;
    void *reference;
    u8 bounds[1];
};
struct ProximityPlayer {
    u8 unknown0[0x30];
    struct ContactPosition position;
};
s32 Nav_IsPositionInMesh(void *, void *, void *);
u32 NavMesh_FindRoute(void *, void *, void *);
u32 ProximityVolume_ContainsPlayer(struct ProximityVolume *volume)
{
    struct ContactPosition *player = &(*(struct ProximityPlayer **)gPlayers)->position;
    void *bounds = volume->bounds;
    struct ContactPosition *position = &volume->position;
    if (Nav_IsPositionInMesh(bounds, position, player) && NavMesh_FindRoute(bounds, position, player))
        return 1;
    return 0;
}
