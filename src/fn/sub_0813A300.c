#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SpritePairPool_Initialize(u8 *);
void Task_Kill(u8 *);
void PairCallbackPool_Update(void);
void FollowingSpriteEffect_DestroyManager(void);

u8 *FollowingSpriteEffect_EnsureManager(void)
{
    u8 *p;
    if (gSpritePairPool != 0)
        return gSpritePairPool;
    p = Task_Create(0xa, 0x1d4);
    if (p != 0) {
        Task_SetCallbacks(p, PairCallbackPool_Update, FollowingSpriteEffect_DestroyManager);
        if (SpritePairPool_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
