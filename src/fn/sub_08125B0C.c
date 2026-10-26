#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneTerrainProbe_InitPool(u8 *);
void Task_Kill(u8 *);
void PairSlotPool_Update(void);
void SceneTerrainProbe_OnPoolDestroy(void);

u8 *SceneTerrainProbe_EnsurePool(void)
{
    u8 *p;
    if (gPairSlotPool != 0)
        return gPairSlotPool;
    p = Task_Create(0xa, 0x1e4);
    if (p != 0) {
        Task_SetCallbacks(p, PairSlotPool_Update, SceneTerrainProbe_OnPoolDestroy);
        if (SceneTerrainProbe_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
