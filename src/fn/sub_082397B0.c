#define ActorEffect_SelectModeFlag sub_082397B0
#include "global.h"
#include "game_state.h"
struct ActorEffectState { u8 unknown0[0x20]; u32 flags; };
static inline u8 global_mode(void)
{
    u32 mask = 2;
    if (gUnk_02000710->flags868 & mask) return 1;
    return 0;
}
s32 sub_082397B0(struct ActorEffectState *s)
{
    s32 result = 0;
    if (!global_mode()) {
        if (s->flags & 16) result = 4;
    } else {
        if (s->flags & 16) result = 8;
    }
    return result;
}
