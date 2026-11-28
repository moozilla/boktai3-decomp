#include "global.h"
#include "ui/results/actor_marker_pool.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 PanelSpritePool_Init(struct MarkerPool *);
void Task_Kill(u8 *);
void ActorMarkerPool_UpdateMarkers(void);
void ActorMarkerPool_Destroy(void);

u8 *ActorMarkerPool_EnsureSingleton(void)
{
    u8 *pool;
    if (gPanelSpritePool != 0)
        return gPanelSpritePool;
    pool = Task_Create(0xa, sizeof(struct MarkerPool));
    if (pool != 0) {
        Task_SetCallbacks(pool, ActorMarkerPool_UpdateMarkers, ActorMarkerPool_Destroy);
        if (PanelSpritePool_Init((struct MarkerPool *)pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
