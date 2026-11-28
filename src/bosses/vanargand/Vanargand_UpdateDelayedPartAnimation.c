#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
u32 Anim_SetSequence(void *, const void *, u32, u32, u32);
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_UpdateDelayedPartAnimation(struct VanargandPairAttack *p)
{
    u32 mask = 4;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 4);
    if (p->pending) {
        p->pending = 0;
        p->phase = 0;
        p->timer = 0;
    }
    switch (p->phase) {
        case 0:
            if (++p->timer > 16) {
                p->phase = 1;
                p->timer = 0;
            }
            break;
        case 1:
            if (p->timer == 0) {
                struct VanargandDamagePart *part = &p->part;
                Anim_SetSequence(&part->sequence, part->sequenceResource, 0, 0, 0);
                p->part.animation = 1;
                part->sequenceId = 0;
            }
            if (++p->timer > 15) {
                u8 next = 2;
                p->phase = next;
                p->timer = 0;
                p->finished = 1;
            }
            break;
        case 2:
        default:
            break;
    }
}
