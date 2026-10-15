#include "route_sprite_actor.h"
#define RouteSpritePool_Create sub_0806E250
s32 sub_0806E130(struct RouteSpritePool *);
s32 sub_0806E1C0(struct RouteSpritePool *);
s32 sub_0806E224(struct RouteSpritePool *);
struct RouteSpritePool *RouteSpritePool_Create(void)
{
    struct RouteSpritePool *pool;
    if (gUnk_02000198)
        return gUnk_02000198;
    pool = Task_Create(10, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))sub_0806E130, (void (*)(void *))sub_0806E1C0);
        if (sub_0806E224(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
