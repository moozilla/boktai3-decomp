#include "monsters/centipede/centipede_body.h"

void Centipede_ResetRouteHistory(struct RoomActor *actor)
{
    struct TentacleBody *body = actor->context;
    u16 *cell = &actor->terrainCell;
    u16 *dst = body->routeHistory;
    s32 i = 5;
    do {
        *dst++ = *cell;
        i--;
    } while (i >= 0);
    body->routeHistoryIndex = 0;
}
