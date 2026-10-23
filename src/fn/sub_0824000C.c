#include "global.h"
#define B(o) (*(u8 *)(p + (o)))
#define H(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
struct TransitionPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void Player_SetSecondaryActionFlags(void *, u32);
void Camera_SnapToWithoutCompletion(void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Player_PlayLocalSong(u32, u32);
u32 ActorVariant_GetTableValue(u32, u32);
void Player_SetAnimationIndex(void *, u32, u32);
void Player_SetTetherMode(void *, u32, u32);
// confidence=low: linked action camera, screen fade and timed animation phases.
#define Player_UpdateLinkedScreenTransition sub_0824000C
void Player_UpdateLinkedScreenTransition(u8 *p)
{
    struct TransitionPosition position;
    Player_SetSecondaryActionFlags(p, 4);
    switch (B(0x458)) {
        case 0:
            position = *(struct TransitionPosition *)(p + 0x30);
            position.y += 150;
            Camera_SnapToWithoutCompletion(&position);
            if (B(0x4E0))
                ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 16);
            else
                ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 1);
            Player_PlayLocalSong(W(0x18), 0x1AD);
            if (B(0x3A4) > 4) {
                s32 frame = (8 - B(0x3A4)) >> 1;
                B(0x3A2) = frame;
                B(0x3A3) = 1;
            } else {
                u32 frame = B(0x3A4) >> 1;
                u8 *target = p + 0x3A2;
                u32 zero = 0;
                *target = frame;
                B(0x3A3) = zero;
            }
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x29), 64);
            Player_SetTetherMode(p, 0x1B, 1);
            H(0x4E8) = 0;
            break;
        case 1:
            if (++H(0x45A) > 5) {
                Player_SetTetherMode(p, 0x1B, 2);
                H(0x4E8) = 0;
                B(0x153) = 1;
                Player_PlayLocalSong(W(0x18), 0x1AE);
            }
            break;
        case 2:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 0x12), 64);
            H(0x4E8) -= 60;
            if (H(0x45A) == 4) {
                if (B(0x418) == 6)
                    Player_PlayLocalSong(W(0x18), 0x2ED);
                else
                    Player_PlayLocalSong(W(0x18), 0x305);
            }
            if (++H(0x45A) > 29)
                Player_SetTetherMode(p, 0x1B, 3);
            break;
        case 3:
            H(0x4E8) = 0;
            Player_SetSecondaryActionFlags(p, 1);
            break;
    }
}
