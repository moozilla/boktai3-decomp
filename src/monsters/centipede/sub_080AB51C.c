#include "monsters/centipede/centipede_body.h"

s32 Centipede_HasVisitedRoute(struct RoomActor *actor, u32 cell)
{
    struct TentacleBody *body = actor->context;
    s32 i = 0;
    u16 *entry = body->routeHistory;
    s32 r;
    do {
        if (*entry == cell) {
            r = 1;
            goto done;
        }
        entry++;
        i++;
    } while (i <= 5);
    r = 0;
done:
    return r;
}
