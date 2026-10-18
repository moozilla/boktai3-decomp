#include "global.h"

extern void *gUnk_02000238;
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 Hresvelgr_InitShotPool(void *, s32, s32, s32);
void Task_Kill(void *);
void Hresvelgr_UpdateShots(void);
void Hresvelgr_DestroyShotPool(void);

void *Hresvelgr_CreateShotPool(s32 a, s32 b, s32 c)
{
    void *r;
    if (gUnk_02000238 != 0)
        return gUnk_02000238;
    gUnk_02000238 = Task_Create(8, 0x1248);
    r = gUnk_02000238;
    if (r != 0) {
        Task_SetCallbacks(r, Hresvelgr_UpdateShots, Hresvelgr_DestroyShotPool);
        if (Hresvelgr_InitShotPool(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
