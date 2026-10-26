#include "global.h"
#include "ram.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081A4DF0(void *, s32, s32, s32);
void Task_Kill(void *);
void Hresvelgr_UpdateProjectilePool(void);
void Hresvelgr_DestroyProjectilePool(void);

void *Hresvelgr_CreateProjectilePool(s32 a, s32 b, s32 c)
{
    void *r;
    if (gHresvelgrProjectilePool != 0)
        return gHresvelgrProjectilePool;
    gHresvelgrProjectilePool = Task_Create(8, 0xEE4);
    r = gHresvelgrProjectilePool;
    if (r != 0) {
        Task_SetCallbacks(r, Hresvelgr_UpdateProjectilePool, Hresvelgr_DestroyProjectilePool);
        if (sub_081A4DF0(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
