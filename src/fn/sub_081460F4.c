#include "global.h"

s32 Player_SetAnimation(u8 *, s32, s32, s32);
void Player_SetActionPhase(u8 *p, u8 v);
void Player_AddControlFlags(u8 *, s32);
s32 ActorVariant_GetTableValue(u32, s32);
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(s32);
extern u32 gEngineFlags;

#define F8(o)  (*(u8 *)(p + (o)))
#define F16(o) (*(u16 *)(p + (o)))

void Player_UpdateDeathSequence(u8 *p)
{
    Player_AddControlFlags(p, 4);
    switch (F8(0x458)) {
        case 0: {
            u32 z;
            u8 *q;
            Player_SetAnimation(p, ActorVariant_GetTableValue(F8(0x418), 0x29), 0x40, 1);
            Sound_PlaySong(0x1AD);
            q = p + 0x153;
            z = 0;
            *q = 1;
            Player_SetActionPhase(p, 1);
            F16(0x4E8) = z;
            *(u32 *)(p + 0x40) = z;
            break;
        }
        case 1:
            F16(0x45A) = F16(0x45A) + 1;
            if (F16(0x45A) > 0xe) {
                ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0x1FFF, 2);
                F16(0x4E8) = 0;
                Player_SetActionPhase(p, 2);
            }
            break;
        case 2:
            if (F16(0x45A) == 9) {
                if (F8(0x418) == 6)
                    Sound_PlaySong(0x2EC);
                else
                    Sound_PlaySong(0x304);
            }
            F16(0x45A) = F16(0x45A) + 1;
            if (F16(0x45A) > 0xe) {
                Player_SetActionPhase(p, 3);
                F16(0x4E8) = 0;
                Sound_PlaySong(0x1AF);
            }
            break;
        case 3:
            Player_SetAnimation(p, ActorVariant_GetTableValue(F8(0x418), 0x12), 0x40, 1);
            F16(0x4E8) = F16(0x4E8) - 0x1E;
            F16(0x45A) = F16(0x45A) + 1;
            if (F16(0x45A) > 0x27) {
                F16(0x424) = 0;
                Player_SetActionPhase(p, 4);
            } else {
                Sprite_BlinkUntilExpired(p + 0x14C, F16(0x45A), 0xA, 0x14);
            }
            break;
        case 4:
            F16(0x4E8) = 0;
            Player_AddControlFlags(p, 1);
            F16(0x45A) = F16(0x45A) + 1;
            if (F16(0x45A) > 0x3b) {
                Player_SetActionPhase(p, 5);
                gEngineFlags |= 0x400;
            }
            break;
        case 5:
            Player_AddControlFlags(p, 1);
            break;
    }
}
