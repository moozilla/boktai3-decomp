#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
#include "engine/input.h"

// confidence=low: cycles five auxiliary-animation debug modes on a key press.
void Debug_NudgeWorldPositionWithA(void *);
void Debug_NudgeWorldPositionWithB(void *);
void Anim_SetSequence(void *, const void *, s32, s32, s32);
void Vanargand_UpdatePartDebugControls(struct VanargandPairAttack *p)
{
    struct VanargandDamagePart *part = &p->part;
    if (p->pending) {
        p->pending = 0;
        p->phase = 0;
        p->timer = 0;
    }
    Debug_NudgeWorldPositionWithA((u8 *)p + 0x418);
    Debug_NudgeWorldPositionWithB((u8 *)p + 0x478);
    if (gPadState[1] & 0x100) {
        p->phase = (p->phase + 1) % 5;
        p->timer = 0;
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                part->sprite.frame = 7;
                part->animation = 0;
                p->timer++;
            }
            break;
        case 1:
            if (p->timer == 0) {
                Anim_SetSequence(&part->sequence, part->sequenceResource, 0, 0, 0);
                part->animation = 1;
                part->sequenceId = 0;
                p->timer++;
            }
            break;
        case 2:
            if (p->timer == 0) {
                u32 mode = 4;
                Anim_SetSequence(&part->sequence, part->sequenceResource, 0, 0, mode);
                part->animation = 1;
                part->sequenceId = 0;
                p->timer++;
            } else {
                break;
            }
        case 3:
            if (p->timer == 0) {
                u32 one = 1;
                Anim_SetSequence(&part->sequence, part->sequenceResource, one, 0, 0);
                part->animation = one;
                part->sequenceId = 0;
                p->timer++;
            }
            break;
        case 4:
            if (p->timer == 0) {
                Anim_SetSequence(&part->sequence, part->sequenceResource, 2, 0, 0);
                part->animation = 1;
                part->sequenceId = 0;
                p->timer++;
            }
            break;
    }
}
