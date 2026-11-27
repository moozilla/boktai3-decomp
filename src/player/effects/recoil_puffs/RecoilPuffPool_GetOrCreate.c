#include "global.h"
#include "engine/ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 RecoilPuffPool_Init(u8 *);
void Task_Kill(u8 *);
void RecoilPuffPool_UpdateTask(void);
void RecoilPuffPool_DestroyTask(void);

u8 *RecoilPuffPool_GetOrCreate(void)
{
    u8 *p;
    if (gRisingSpritePool != 0)
        return gRisingSpritePool;
    p = Task_Create(0xa, 0x19c);
    if (p != 0) {
        Task_SetCallbacks(p, RecoilPuffPool_UpdateTask, RecoilPuffPool_DestroyTask);
        if (RecoilPuffPool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
