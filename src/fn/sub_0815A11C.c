#define Player_AdvanceTetherPhase sub_0815A11C
#define Player_SetTetherPhase Player_SetActionPhase
#include "global.h"
#include "player.h"
void Player_SetActionPhase(struct Player *, u8);
void sub_0815A11C(struct Player *p)
{
    if (p->tetherMode == 0x1F) {
        if (p->tetherPhase == 3)
            Player_SetActionPhase(p, 4);
    } else if (p->tetherMode == 0x20) {
        if (p->tetherPhase == 4)
            Player_SetActionPhase(p, 5);
    } else if (p->tetherMode == 0x24) {
        if (p->actionMode == 6) {
            if (p->tetherPhase == 3)
                Player_SetActionPhase(p, 4);
        } else {
            if (p->tetherPhase == 1)
                Player_SetActionPhase(p, 2);
        }
    } else if (p->kind == 2 && p->tetherMode == 0x15 && p->tetherPhase == 1) {
        Player_SetActionPhase(p, 4);
    }
}
