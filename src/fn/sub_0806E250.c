#include "route_sprite_actor.h"
s32 RouteSpritePool_Tick(struct RouteSpritePool *);
s32 RouteSpritePool_Destroy(struct RouteSpritePool *);
s32 RouteSpritePool_Initialize(struct RouteSpritePool *);
struct RouteSpritePool *RouteSpritePool_Create(void)
{
    struct RouteSpritePool *pool;
    if (gRouteSpritePool)
        return gRouteSpritePool;
    pool = Task_Create(10, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))RouteSpritePool_Tick,
                          (void (*)(void *))RouteSpritePool_Destroy);
        if (RouteSpritePool_Initialize(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
