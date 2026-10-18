#include "global.h"
/* Effect pool family names and field interpretations are tentative. */
#define EffectPool_Create              sub_081A3F84
#define EffectPool_Initialize          sub_081A3F04
#define EffectPool_UpdateActiveEntries sub_081A3D00
#define EffectPool_Destroy Hresvelgr_DestroyHomingShotPool
#define gEffectPool                    gUnk_0200023C
extern void *gUnk_0200023C;
void *Task_Create(u32, u32);
void Task_Kill(void *);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void sub_081A3D00(void);
void Hresvelgr_DestroyHomingShotPool(void);
s32 sub_081A3F04(void *, s32, s32, s32);
void *sub_081A3F84(s32 a, s32 b, s32 c)
{
    void *pool;
    if (gEffectPool)
        return gEffectPool;
    gEffectPool = Task_Create(8, 0x4b8);
    pool = gEffectPool;
    if (pool) {
        Task_SetCallbacks(pool, EffectPool_UpdateActiveEntries, EffectPool_Destroy);
        if (EffectPool_Initialize(pool, a, b, c) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
