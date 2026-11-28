#include "monsters/centipede/centipede_body.h"

s32 Centipede_HasVisitedRoute(struct RoomActor *actor, u32 cell)
{
    struct TentacleBody *body = actor->context;
    s32 i;
    for (i = 0; i < 6; i++) {
        if (body->routeHistory[i] == cell)
            return 1;
    }
    return 0;
}
