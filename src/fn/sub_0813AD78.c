#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SpriteParticle_InitManager(u8 *);
void Task_Kill(u8 *);
void ScreenMote_UpdateManager(void);
void EffectPool_Destroy(void);

u8 *SpriteParticle_GetOrCreateManager(void)
{
    u8 *p;
    if (gScreenMoteManager != 0)
        return gScreenMoteManager;
    p = Task_Create(0xa, 0x99c);
    if (p != 0) {
        Task_SetCallbacks(p, ScreenMote_UpdateManager, EffectPool_Destroy);
        if (SpriteParticle_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
