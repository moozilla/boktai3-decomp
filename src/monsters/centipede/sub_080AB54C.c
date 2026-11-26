#include "monsters/centipede/centipede_body.h"

void Centipede_RecordVisitedRoute(struct RoomActor *actor, u16 cell)
{
    struct TentacleBody *body = actor->context;
    body->routeHistory[body->routeHistoryIndex] = cell;
    body->routeHistoryIndex = (body->routeHistoryIndex + 1) & 3;
}
