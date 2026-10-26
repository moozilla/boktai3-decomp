#include "global.h"
#include "engine.h"

// confidence=low: timed animation and screen-transition sequence; gameplay trigger is unknown.
#define Player_UpdateTimedScreenTransition sub_081462DC
#define B(n)                               (*(u8 *)(p + (n)))
#define H(n)                               (*(u16 *)(p + (n)))
void Player_AddControlFlags(void *, u32);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Sound_PlaySong(u32);
void Player_SetActionPhase(void *, u8);
void sub_08232420(s32, s32, s32, s32, void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Player_UpdateTimedScreenTransition(u8 *p)
{
    Player_AddControlFlags(p, 4);
    switch (B(0x458)) {
        case 0: {
            u32 zero;
            u8 *flag;
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 41), 64, 1);
            Sound_PlaySong(0x1AD);
            flag = p + 0x153;
            zero = 0;
            *flag = 1;
            Player_SetActionPhase(p, 1);
            H(0x4E8) = zero;
            *(u32 *)(p + 0x40) = zero;
            break;
        }
        case 1:
            if (++H(0x45A) > 14) {
                Player_AddControlFlags(p, 1);
                Sound_PlaySong(0x1AE);
                Player_SetActionPhase(p, 2);
            }
            break;
        case 2:
            H(0x4E8) -= 30;
            if (++H(0x45A) > 3) {
                sub_08232420(1, 0, 0, 30, p + 0x30);
                Sound_PlaySong(0x145);
                H(0x424) = 0;
                Player_SetActionPhase(p, 3);
            }
            break;
        case 3:
            Player_AddControlFlags(p, 1);
            if (++H(0x45A) > 29) {
                ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0x1FFF, 2);
                Player_SetActionPhase(p, 4);
            }
            break;
        case 4:
            Player_AddControlFlags(p, 1);
            if (++H(0x45A) > 29) {
                Player_SetActionPhase(p, 5);
                gEngineFlags |= 0x400;
            }
            break;
        case 5:
            Player_AddControlFlags(p, 1);
            break;
    }
}
