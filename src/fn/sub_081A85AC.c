#include "vanargand_pair_attack.h"
#include "player.h"

void Vanargand_UpdateTargetFlagTransition(struct VanargandPairAttack *p)
{
    if (Vanargand_ConsumeActionEntry(p) && p->state != 17)
        Vanargand_EnterAction(p, 17);
    if (p->finished) {
        struct Player *player = p->player;
        u32 mask = 0x1000;
        u32 flags = player->statusFlags & mask;
        if (!flags) {
            u32 phase = 13;
            p->actionPhase = phase;
            p->actionTimer = flags;
            p->entered = 1;
            return;
        } else {
            Vanargand_EnterActionPhase(p, 17);
        }
    }
}
