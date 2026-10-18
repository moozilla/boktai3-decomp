#include "global.h"

extern u8 *gUnk_0200013C;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneActor14_InitManager(u8 *);
void Task_Kill(u8 *);
void SpriteEffectPool_Update(void);
void SpriteEffectPool_RemoveSlots(void);

u8 *SceneActor14_EnsureSingleton(void)
{
    u8 *p;
    if (gUnk_0200013C != 0)
        return gUnk_0200013C;
    p = Task_Create(0xa, 0x6f8);
    if (p != 0) {
        Task_SetCallbacks(p, SpriteEffectPool_Update, SpriteEffectPool_RemoveSlots);
        if (SceneActor14_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
