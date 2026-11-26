#include "bosses/vanargand/vanargand_pair_attack.h"
#include "engine/shared/player.h"

static inline u8 Vanargand_IsPlayerReadyMode(u8 mode)
{
    if (mode == 0 || mode == 2)
        return TRUE;
    return FALSE;
}

void Vanargand_UpdateMode3Transition(void *object)
{
    struct VanargandPairAttack *p = object;
    u32 consumed = Vanargand_ConsumeActionEntry(p);
    if (consumed) {
        u8 mode = p->player->actionMode;
        ((struct VanargandPairAttack *)object)->actionSequence.playerReady =
            Vanargand_IsPlayerReadyMode(mode);
        Vanargand_EnterAction(p, 3);
    } else if (p->finished) {
        if (((struct VanargandPairAttack *)object)->mode == 0)
            Vanargand_EnterActionPhase(p, 14);
        else
            Vanargand_EnterActionPhase(p, 11);
    }
    p->actionTimer++;
}
