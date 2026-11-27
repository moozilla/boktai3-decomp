#include "player/effects/recoil_puff_sprite.h"
#include "engine/ram.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);

struct RecoilPuffPool *RecoilPuffPool_GetOrCreate(void)
{
    struct RecoilPuffPool *p;
    if (gRisingSpritePool != 0)
        return (struct RecoilPuffPool *)gRisingSpritePool;
    p = Task_Create(0xa, sizeof(*p));
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void))RecoilPuffPool_UpdateTask,
                          (void (*)(void))RecoilPuffPool_DestroyTask);
        if (RecoilPuffPool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
