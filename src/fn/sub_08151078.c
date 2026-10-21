#include "global.h"
#include "player.h"

void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void Player_UpdateFacingSide(void *);
extern const s16 gUnk_086149C4[];

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void PlayerCue_UpdateRecoilRecovery(struct Player *p)
{
    s32 value, angle;
    s32 phase = p->tetherPhase;
    switch (phase) {
        case 0: {
            u32 frame = p->tetherFrame;
            if (!frame) {
                if ((u8)(s8)p->facing > 4) {
                    p->heading = (8 - p->facing) >> 1;
                    p->side = 1;
                } else {
                    p->heading = (u8)(s8)p->facing >> 1;
                    p->side = frame;
                }
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40, 1);
                if (p->actionMode != 6)
                    Sound_PlaySong(0xE7);
            }
        }
            angle = ((p->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 0x40) & 0xFF] * p->recoilSpeed;
            p->velocityX = -scale(value);
            value = gUnk_086149C4[angle] * p->recoilSpeed;
            p->velocityZ = -scale(value);
            value = (p->recoilSpeed * 3) >> 2;
            p->recoilSpeed = value;
            if (!value)
                ActorSequence_Begin(p, 0x1D, 1);
            break;
        case 1:
            if (p->actionCueFlags == 3 && p->actionCueCounter == 0)
                Sound_PlaySong(0xD1);
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2E), 0x40, 1)) {
                ActorSequence_Begin(p, 0x1D, 2);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 2:
            break;
    }
}
