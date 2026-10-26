#include "global.h"
#include "player.h"
#include "game_state.h"
s32 AreaEffect_IsPositionCovered(void *);
static inline u8 Player_TestSceneFeature(u32 mask)
{
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}
static inline u32 Player_TestBonusFlag(u32 *flags, u32 mask)
{
    return *flags & mask;
}
s32 Player_CanUseSceneFeature(struct Player *p)
{
    if (!Player_TestSceneFeature(0x1000))
        return 0;
    if (p && Player_TestBonusFlag(&p->statBonusFlags, 0x800))
        return 0;
    if (!Player_TestSceneFeature(2) && !AreaEffect_IsPositionCovered(&p->position))
        return 0;
    return 1;
}
