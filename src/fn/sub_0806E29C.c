// COMPILER: old_agbcc
#include "route_sprite_actor.h"
/* RouteSpritePool_ResetSingleton: Clears the route sprite pool singleton pointer */
#define RouteSpritePool_ResetSingleton sub_0806E29C
#define RouteSpritePool_ResetSingleton sub_0806E29C
void RouteSpritePool_ResetSingleton(void)
{
    gRouteSpritePool = 0;
}
