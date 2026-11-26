#include "global.h"
#include "engine/ram.h"
/* Effect pool family names and field interpretations are tentative. */
void *Task_Create(u32, u32);
void Task_Kill(void *);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void HresvelgrEffectPool_UpdateActiveEntries(void);
void Hresvelgr_DestroyHomingShotPool(void);
s32 HresvelgrEffectPool_Initialize(void *, s32, s32, s32);
void *HresvelgrEffectPool_Create(s32 a, s32 b, s32 c)
{
    void *pool;
    if (gEffectPoolManager)
        return gEffectPoolManager;
    gEffectPoolManager = Task_Create(8, 0x4b8);
    pool = gEffectPoolManager;
    if (pool) {
        Task_SetCallbacks(pool, HresvelgrEffectPool_UpdateActiveEntries, Hresvelgr_DestroyHomingShotPool);
        if (HresvelgrEffectPool_Initialize(pool, a, b, c) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
