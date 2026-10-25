#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 DirectionalSpritePool_Init(void *);
void Task_Kill(void *);
void PlayerClone_UpdatePool(void);
void PlayerClone_DestroyPool(void);

void *PlayerClone_CreatePool(void)
{
    void *p = Task_Create(8, 0xa2c);
    if (p) {
        Task_SetCallbacks(p, PlayerClone_UpdatePool, PlayerClone_DestroyPool);
        if (DirectionalSpritePool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        return p;
    }
}
