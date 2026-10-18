#define Player_UpdateMovementFacing sub_0814433C
#include "global.h"
#include "player.h"
void sub_0814198C(struct Player *, struct SceneVec *, s32, s32, s32, s32, s32);
void sub_0813ADD0(struct Player *);
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(struct Player *, s32, s32, s32);
void sub_0813AFEC(struct Player *, s32, s32);
void sub_0814433C(struct Player *p)
{
    if (p->directionHistory[0] >= 0 && p->facing != p->directionHistory[0]) {
        if (p->facingChangeEffects & 1) {
            if ((p->facingChangeEffects >> 1) & 1)
                sub_0814198C(p, &p->position, 1, 1, 0, 0, 0x116);
            else
                sub_0814198C(p, &p->position, 1, 1, 0, 0, 0x117);
        }
        p->facing = p->directionHistory[0];
        sub_0813ADD0(p);
    }
    sub_0813AE18(p, sub_081419D4(p->actionMode, 0), 64, 1);
    if ((s8)p->movementAnimationState < 0)
        sub_0813AFEC(p, 0, 0);
}
