#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void EnemyD459_Update(void);
void EnemyD459_Destroy(void);
s32 EnemyD459_InitializeFromScript(void *, void *);
void *EnemyD459_Create(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, EnemyD459_Update, EnemyD459_Destroy);
        if (EnemyD459_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
