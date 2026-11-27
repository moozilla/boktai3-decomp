#include "monsters/centipede/centipede_body.h"

void Centipede_ResetRouteHistory(struct RoomActor *actor)
{
    struct TentacleBody *body = actor->context;
    s32 i;
    for (i = 0; i < 6; i++)
        body->routeHistory[i] = actor->terrainCell;
    body->routeHistoryIndex = 0;
}
