#include "route_sprite_actor.h"
#define RouteSpritePool_Initialize sub_0806E224
s32 RouteSpritePool_Initialize(struct RouteSpritePool *pool)
{
    pool->sequenceResource = ScriptProc_Start(0x922E, 0x1DC3);
    pool->occupiedMask = 0;
    gUnk_02000198 = pool;
    return 0;
}
