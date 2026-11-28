#include "bosses/vanargand/vanargand_pair_attack.h"
#include "player/player.h"

void Vanargand_UpdateTargetFlagTransition(struct VanargandPairAttack *p)
{
    if (Vanargand_ConsumeActionEntry(p) && p->state != 17)
        Vanargand_EnterAction(p, 17);
    if (p->finished) {
        struct Player *player = p->player;
        u32 mask = 0x1000;
        u32 flags = player->statusFlags & mask;
        if (!flags) {
            Vanargand_EnterActionPhase(p, 13);
            return;
        } else {
            Vanargand_EnterActionPhase(p, 17);
        }
    }
}
