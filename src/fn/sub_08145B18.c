#include "global.h"
#include "player.h"
#include "engine.h"

void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
u32 Player_ConsumeStateEntry(struct Player *);
void WirelessGame_NotifyPlayerDefeated(void);
void Player_AddControlFlags(struct Player *, u32);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u8, u32, u32, u32, u32, u32);
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
// confidence=low: defeat notification and recoil lead to a delayed fade phase.
void Player_UpdateDefeatSlide(struct Player *p)
{
    s32 value, angle;
    if (Player_ConsumeStateEntry(p)) {
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 64, 1);
        p->interactionResetCounter = 0;
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        Player_SetActionPhase(p, 0);
        if (p->actionMode == 6)
            Sound_PlaySong(0x2EB);
        else
            Sound_PlaySong(0xE7);
        Sound_PlaySong(0xE6);
        if (gEngineFlags & 0x1000)
            WirelessGame_NotifyPlayerDefeated();
        p->kindTwoAttachment =
            ActorAttachment_UpdateFromDescriptor(p->kindTwoAttachment, &p->positionalEffectGroup,
                                                 p->kindTwoAttachmentDescriptor, 0x7F, 0, 0, 0, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            angle = ((p->facing + 1) & 7) << 5;
            value = gUnk_086149C4[(angle + 64) & 255] * p->directionFrame;
            p->velocityX = scale(value);
            value = gUnk_086149C4[angle] * p->directionFrame;
            p->velocityZ = scale(value);
            p->directionFrame = (p->directionFrame * 3) >> 2;
            if (!p->directionFrame)
                Player_SetActionPhase(p, 1);
            break;
        case 1:
            if (p->actionCueFlags == 3 && !p->actionCueCounter)
                Sound_PlaySong(0xD1);
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2E), 64, 1)) {
                if (!(gEngineFlags & 0x1000))
                    Player_SetActionPhase(p, 2);
                else
                    Player_SetActionPhase(p, 3);
            }
            break;
        case 2:
            if (++p->tetherFrame > 15) {
                gEngineFlags |= 0x400;
                Player_SetActionPhase(p, 3);
            }
            break;
        case 3:
            if (++p->tetherFrame > 15)
                *((u8 *)p + 0xA6) = 1;
            break;
        case 4:
            if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 3))
                Player_AddControlFlags(p, 5);
            if (++p->tetherFrame > 55)
                Player_SetActionPhase(p, 5);
            break;
        case 5:
            Player_AddControlFlags(p, 5);
            break;
    }
}
