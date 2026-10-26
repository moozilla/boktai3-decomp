#include "global.h"
#include "ram.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 Hresvelgr_InitShotPool(void *, s32, s32, s32);
void Task_Kill(void *);
void Hresvelgr_UpdateShots(void);
void Hresvelgr_DestroyShotPool(void);

void *Hresvelgr_CreateShotPool(s32 a, s32 b, s32 c)
{
    void *r;
    if (gDirectionBurstEmitter != 0)
        return gDirectionBurstEmitter;
    gDirectionBurstEmitter = Task_Create(8, 0x1248);
    r = gDirectionBurstEmitter;
    if (r != 0) {
        Task_SetCallbacks(r, Hresvelgr_UpdateShots, Hresvelgr_DestroyShotPool);
        if (Hresvelgr_InitShotPool(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
