#include "global.h"

struct P081561A8 {
    u8 f0[0x458];
    u8 f458;
};
s32 sub_0813AE18(void *, u32, u32, u32);
void sub_0813ADD0(void *);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);

void PlayerCue_UpdateStandDiagonalB(struct P081561A8 *p)
{
    if (sub_0813AE18(p, 0x1BB, 0x40, 1) != 0) {
        p->f458++;
        if (p->f458 > 1) {
            sub_0813ADD0(p);
            PlayerCue_EnterStandUpdate(p);
            PlayerCue_CompleteAction(p);
        }
    }
}
