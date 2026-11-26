// COMPILER: old_agbcc
#include "scene/motion/route_sprite_actor.h"
/* RouteSpritePool_ResetSingleton: Clears the route sprite pool singleton pointer */
void RouteSpritePool_ResetSingleton(void)
{
    gRouteSpritePool = 0;
}
