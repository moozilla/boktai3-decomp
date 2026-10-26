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
void Player_UpdateDefeatRecoil(struct Player *p)
{
    s32 value, angle;
    if (Player_ConsumeStateEntry(p)) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
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
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40, 1);
            if (++p->tetherFrame > 3) {
                Player_SetActionPhase(p, 1);
                p->terrainRecoveryCounter = 300;
            }
            break;
        case 1:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2B), 0x40, 1);
            angle = ((p->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 64) & 255] * p->directionFrame;
            p->velocityX = -scale(value);
            value = gUnk_086149C4[angle] * p->directionFrame;
            p->velocityZ = -scale(value);
            p->directionFrame = (p->directionFrame * 3) >> 2;
            if (p->terrainRecoveryCounter > 14)
                p->terrainRecoveryCounter -= 15;
            else
                p->terrainRecoveryCounter = 0;
            if (p->directionFrame <= 19) {
                Player_SetActionPhase(p, 2);
                p->terrainRecoveryCounter = 0;
                Sound_PlaySong(0x2A8);
            }
            break;
        case 2:
            angle = ((p->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 64) & 255] * p->directionFrame;
            p->velocityX = -scale(value);
            value = gUnk_086149C4[angle] * p->directionFrame;
            p->velocityZ = -scale(value);
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2C), 0x40, 1)) {
                p->directionFrame = 0;
                if (!(gEngineFlags & 0x1000))
                    Player_SetActionPhase(p, 3);
                else
                    Player_SetActionPhase(p, 4);
            }
            break;
        case 3:
            if (++p->tetherFrame > 19) {
                gEngineFlags |= 0x400;
                Player_SetActionPhase(p, 4);
            }
            break;
        case 4:
            if (++p->tetherFrame > 15)
                *((u8 *)p + 0xA6) = 1;
            break;
        case 5:
            if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 3))
                Player_AddControlFlags(p, 5);
            if (++p->tetherFrame > 55)
                Player_SetActionPhase(p, 6);
            break;
        case 6:
            Player_AddControlFlags(p, 5);
            break;
    }
}
