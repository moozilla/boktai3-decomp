#include "global.h"
#include "engine/ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 LootDepositPool_InitManager(u8 *);
void Task_Kill(u8 *);
void CallbackPool_UpdateEntries(void);
void CallbackPool_DestroyEntries(void);

u8 *LootDepositPool_CreateManager(void)
{
    u8 *p;
    if (gActorGroupPool != 0)
        return gActorGroupPool;
    p = Task_Create(0xa, 0x630);
    if (p != 0) {
        Task_SetCallbacks(p, CallbackPool_UpdateEntries, CallbackPool_DestroyEntries);
        if (LootDepositPool_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
