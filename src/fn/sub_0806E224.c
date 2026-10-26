#include "route_sprite_actor.h"
s32 RouteSpritePool_Initialize(struct RouteSpritePool *pool)
{
    pool->sequenceResource = ScriptProc_Start(0x922E, 0x1DC3);
    pool->occupiedMask = 0;
    gRouteSpritePool = pool;
    return 0;
}
