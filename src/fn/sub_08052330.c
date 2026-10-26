#include "stacked_actor.h"
s32 StackedActor_IsPlayerNearby(struct StackedActor *o)
{
    struct GameState *g = gGameState;
    s32 dx = g->positionX - o->world.x;
    s32 dy;
    if (dx < 0)
        dx = -dx;
    dy = g->positionZ - o->world.z;
    if (dy < 0)
        dy = -dy;
    if (dx > 0x60 || dy > 0x60)
        return 0;
    if (dx * dx + dy * dy > 0x2400)
        return 0;
    return 1;
}
