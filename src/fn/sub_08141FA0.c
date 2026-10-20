#define Player_UpdateResourceDash                                                                  \
    sub_08141FA0 // confidence=low: resource-gated timed directional movement
#include "global.h"
#include "player.h"
extern u8 *gUnk_02000710;
s32 Player_ConsumeStateEntry(struct Player *);
s32 Player_CanSpendSharedResource(struct Player *, s32);
void Sound_PlaySong(s32);
s32 Div(s32, s32);
void Player_SetActionPhase(struct Player *, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
s32 Player_GetSmoothedDirection(struct Player *);
void Player_SetDirectionalVelocity(struct Player *, s32);
void sub_08141C40(struct Player *, s32);
void Player_UpdateFacingSide(struct Player *);
s32 ActorVariant_GetTableValue(s32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
static inline u32 Mask(u32 *f, u32 m)
{
    return *f & m;
}
static inline u32 FreeDash(struct Player *p)
{
    if (!p)
        return 0;
    return Mask(&p->actionFlags, 32);
}
void Player_UpdateResourceDash(struct Player *p)
{

    if (Player_ConsumeStateEntry(p)) {
        if (Player_CanSpendSharedResource(p, 1) || FreeDash(p)) {
            Sound_PlaySong(0x392);
            if (FreeDash(p))
                p->unk4EF[1] = 26;
            else
                p->unk4EF[1] = *(s16 *)(gUnk_02000710 + 0x876) * 2 + 14;
            if (p->unk4EF[1] > 32)
                p->unk4EF[1] = 32;
            p->unk4EF[2] = Div(512, p->unk4EF[1]);
            Player_SetActionPhase(p, 0);
        } else {
            Sound_PlaySong(0x192);
            Player_SetActionPhase(p, 1);
        }
    }
    switch (p->tetherPhase) {
        case 0:
            if (++p->tetherFrame >= p->unk4EF[2]) {
                ActorSequence_Begin(p, 0, 0);
                break;
            }
            if (p->tetherFrame < p->unk4EF[2] - 8) {
                p->actionEighteenParameter = 0x172;
                p->actionEighteenDuration = 32;
            }
            if ((s16)p->directionHistory[0] != -1 && !p->cameraOffsetActive)
                p->facing = Player_GetSmoothedDirection(p);
            Player_SetDirectionalVelocity(p, p->unk4EF[1]);
            sub_08141C40(p, 1);
            Player_UpdateFacingSide(p);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 1), 32, 1);
            break;
        case 1:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            if (++p->tetherFrame > 7)
                ActorSequence_Begin(p, 0, 0);
            break;
    }
}
