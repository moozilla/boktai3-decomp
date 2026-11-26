#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ProjectilePool_Init(void *);
void Task_Kill(void *);
void ProjectilePool_Update(void);
void ProjectilePool_Destroy(void);
void *ProjectilePool_Create(void)
{
    void *p = Task_Create(8, 0x129C);
    if (p) {
        Task_SetCallbacks(p, ProjectilePool_Update, ProjectilePool_Destroy);
        if (ProjectilePool_Init(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
