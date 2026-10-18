#define Player_UpdateTimedCuePhase   sub_08146478
#define Player_SelectActionAnimation sub_081419D4
#define Player_StepActionAnimation   sub_0813AE18
#define Player_CallActionScript Player_CallEventScript
#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(struct Player *, s32, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
void Player_CallEventScript(struct Player *, u32);
void sub_08146478(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0:
            if (p->tetherFrame == 0) {
                sub_0813AE18(p, sub_081419D4(p->actionMode, 0x29), 0x40, 1);
                if (p->actionMode == 6)
                    Sound_PlaySong(0x2EE);
                else
                    Sound_PlaySong(0x301);
            }
            if (++p->tetherFrame > 5)
                Player_SetActionPhase(p, 1);
            break;
        case 1:
            if (!(p->actionCueFlags & 3) && !p->actionCueCounter)
                Sound_PlaySong(0xD1);
            if (sub_0813AE18(p, sub_081419D4(p->actionMode, 0x2E), 0x40, 1))
                Player_SetActionPhase(p, 2);
            break;
        case 2:
            if (++p->tetherFrame > 15) {
                Player_SetActionPhase(p, 3);
                Player_CallEventScript(p, 1);
            }
            break;
    }
}
