#include "bosses/vanargand/vanargand_pair_attack.h"
u32 Vanargand_IsEitherComponentFlag40Set(void *);
s32 VanargandPair_RequestPairReaction(struct VanargandPairAttack *panel)
{
    u32 two = 2;
    if (panel->flags & two) {
        Vanargand_StartAction(panel, 1, 0, 6);
        return 1;
    }
    if (panel->flags & 0x10000) {
        Vanargand_StartAction(panel, 1, 0, 3);
        return 1;
    }
    if (panel->flags & 1) {
        if (ScenePair_CountPositiveCounters(panel->pair) == 0)
            Vanargand_StartAction(panel, 1, 0, 1);
        else
            Vanargand_StartAction(panel, 1, 0, 0);
        return 1;
    }
    if (Vanargand_IsEitherComponentFlag40Set(panel->pair)) {
        Vanargand_StartAction(panel, 0, 0, two);
        return 1;
    }
    if (ScenePair_CountPositiveCounters(panel->pair) == 0 && panel->mode != 1 &&
        panel->actionPhase != 11 && panel->actionPhase != 14 && panel->actionPhase != 1) {
        Vanargand_StartAction(panel, 0, 0, 11);
        return 1;
    }
    return 0;
}
