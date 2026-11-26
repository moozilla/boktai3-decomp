#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void EnemyF426_UpdateDamageState(void);
void EnemyF426_Destroy(void);
s32 EnemyF426_InitializeFromScript(void *, void *);
void *EnemyF426_Create(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, EnemyF426_UpdateDamageState, EnemyF426_Destroy);
        if (EnemyF426_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
