#include "global.h"
#include "player.h"
#include "engine.h"
// confidence=low: charge motion pose with a countdown that shrinks the attachment descriptor.
void Sound_PlaySong(u32);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, int, int, int, int);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
u32 Player_ConsumeStateEntry(struct Player *);
void Player_StopActionSounds(struct Player *);
void Player_SetPendingCallback(struct Player *, void (*)(void));
void ActorSequence_Begin(void *, s32, s32);
void Player_OnChargeMotionExit(void);
static inline void SetReset(struct Player *p)
{
    u16 v = 90;
    u16 *dst = &p->positionResetValue;
    *dst = v;
}
void sub_08144D64(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        Player_StopActionSounds(p);
        if (p->positionResetCounter) {
            p->kindTwoAttachmentDescriptor = 3;
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, 3, 0, (int)&p->positionResetTimer,
                (s16)p->positionResetTimer, 0x96, 0xE6);
            Player_SetPendingCallback(p, Player_OnChargeMotionExit);
        }
    }
    if (p->positionResetCounter == 1 && p->positionResetState == 1 && !(gEngineFlags & 0x1000))
        SetReset(p);
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x31), 0x40, 1)) {
                Player_SetActionPhase(p, 1);
                if (p->positionResetCounter)
                    Sound_PlaySong(0xD0);
            }
            break;
        case 1: {
            s32 step;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x32), 0x40, 1);
            step = 1;
            if (p->inputKeys[1] & 0xF3)
                step = 2;
            if (p->positionResetTimer > step) {
                p->positionResetTimer -= step;
            } else {
                p->positionResetTimer = 0;
                Player_SetActionPhase(p, 2);
                p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                    p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor,
                    0x7F, 0, 0, 0, 0);
                if (p->positionResetCounter == 1 && !(gEngineFlags & 0x1000))
                    SetReset(p);
                p->positionResetState = 0;
                p->positionResetCounter = 0;
            }
            break;
        }
        case 2:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x33), 0x40, 1))
                ActorSequence_Begin(p, 0, 0);
            break;
    }
}
