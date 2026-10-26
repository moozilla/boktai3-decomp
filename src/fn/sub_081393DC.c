#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneProcSprites_Initialize(u8 *);
void Task_Kill(u8 *);
void BallisticSpriteEffect_UpdateManager(void);
void BallisticSpriteEffect_DestroyManager(void);

u8 *BallisticSpriteEffect_EnsureManager(void)
{
    u8 *p;
    if (gSceneProcSprites != 0)
        return gSceneProcSprites;
    p = Task_Create(0xa, 0x39c);
    if (p != 0) {
        Task_SetCallbacks(p, BallisticSpriteEffect_UpdateManager,
                          BallisticSpriteEffect_DestroyManager);
        if (SceneProcSprites_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
