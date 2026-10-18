#include "global.h"

struct P081561A8 {
    u8 f0[0x458];
    u8 f458;
};
s32 Player_SetAnimation(void *, u32, u32, u32);
void Player_UpdateFacingSide(void *);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);

void PlayerCue_UpdateStandDiagonalB(struct P081561A8 *p)
{
    if (Player_SetAnimation(p, 0x1BB, 0x40, 1) != 0) {
        p->f458++;
        if (p->f458 > 1) {
            Player_UpdateFacingSide(p);
            PlayerCue_EnterStandUpdate(p);
            PlayerCue_CompleteAction(p);
        }
    }
}
