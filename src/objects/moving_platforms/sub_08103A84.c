#include "global.h"
#include "engine/ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 MovingPlatform_InitManager(u8 *);
void Task_Kill(u8 *);
void MovingPlatform_UpdatePool(void);
void ActorGroup_Destroy(void);

u8 *MovingPlatform_CreateManager(void)
{
    u8 *p;
    if (gNamedActorManager != 0)
        return gNamedActorManager;
    p = Task_Create(5, 0x137c);
    if (p != 0) {
        Task_SetCallbacks(p, MovingPlatform_UpdatePool, ActorGroup_Destroy);
        if (MovingPlatform_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
