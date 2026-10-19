#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 95.6%
// foundry draft (worker best 95.0%); what differs is mostly register allocation and instruction order
#define Player_UpdateRecoilAction sub_08150C5C
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
void sub_08150C5C(struct Player *p)
{
    s32 value, angle;
    switch (p->tetherPhase) {
        case 0: {
            u16 frame = p->tetherFrame;
            if (!frame) {
                s8 direction = p->facing;
                if ((u8)direction > 4) {
                    p->heading = (8 - p->facing) >> 1;
                    p->side = 1;
                } else {
                    p->heading = (u8)direction >> 1;
                    p->side = frame;
                }
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40, 1);
                Sound_PlaySong(0xE6);
                p->tetherFrame++;
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
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2A), 0x40, 1)) {
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08150C5C);
#endif
