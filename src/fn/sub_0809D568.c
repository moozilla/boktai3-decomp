#include "global.h"
void RouteActor_ReflectAtRaisedTerrain(void);
u32 Lun_UpdateTerrainBounce(void)
{
    RouteActor_ReflectAtRaisedTerrain();
    return 1;
}
