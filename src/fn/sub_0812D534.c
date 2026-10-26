#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 FragmentSpritePool_Init(u8 *);
void Task_Kill(u8 *);
void FragmentSpritePool_UpdateTask(void);
void FragmentSpritePool_DestroyTask(void);

u8 *FragmentSpritePool_GetOrCreate(void)
{
    u8 *p;
    if (gScriptedColliderManager != 0)
        return gScriptedColliderManager;
    p = Task_Create(0xa, 0x53c);
    if (p != 0) {
        Task_SetCallbacks(p, FragmentSpritePool_UpdateTask, FragmentSpritePool_DestroyTask);
        if (FragmentSpritePool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
