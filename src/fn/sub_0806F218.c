#include "engine.h"
#include "game_state.h"
static inline void Increment(u16 *value)
{
    if ((s16)*value <= 9998)
        *value = *value + 1;
    else
        *value = 9999;
}
void SceneState_IncrementCounters(void)
{
    s32 next;
    Increment(&gUnk_02000710->counter87C);
    Increment(&gUnk_02000710->counter538);
    Increment((u16 *)((u8 *)gUnk_02000710 + 0x55e));
    next = *(s16 *)((u8 *)gUnk_02000710 + 0x592) + 1;
    if (next > 9999)
        next = 9999;
    else if (next < 0)
        next = 0;
    *(s16 *)((u8 *)gUnk_02000710 + 0x592) = next;
}
