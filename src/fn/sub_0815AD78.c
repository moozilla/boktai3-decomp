#define Player_StopActionModeSounds Player_StopActionSounds
#include "global.h"
#include "player.h"
#include "ram.h"
struct PlayerPendingNotification {
    void *receiver;
    u32 flags;
    void *payload;
};
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);
void sub_08249244(void *, s32, s32, void *);
void Player_StopActionSounds(struct Player *);
void Player_RestoreCameraFollow(struct Player *);
void Player_AddControlFlags(struct Player *, s32);
void Player_EnterKindTwo(struct Player *p)
{
    if (p->kind != 2) {
        p->kindTwoAttachmentCounter = 0;
        p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
            p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor, 0x7F,
            0, 0, 0, 0x50);
    }
    p->kind = 2;
    p->kindTwoState = 0;
    p->kindTwoEntered = 1;
    p->terrainRecoveryCounter = 0;
    p->actionCountdown = 0;
    if ((*(struct PlayerPendingNotification **)&gPlayerPendingActor) &&
        gPlayerNotificationPending) {
        if (!((*(struct PlayerPendingNotification **)&gPlayerPendingActor)->flags & 1)) {
            if ((*(struct PlayerPendingNotification **)&gPlayerPendingActor)->flags & 0x10)
                sub_08249244((*(struct PlayerPendingNotification **)&gPlayerPendingActor)->receiver,
                             1, 60,
                             (*(struct PlayerPendingNotification **)&gPlayerPendingActor)->payload);
            else
                sub_08249244((*(struct PlayerPendingNotification **)&gPlayerPendingActor)->receiver,
                             1, 60,
                             (*(struct PlayerPendingNotification **)&gPlayerPendingActor)->payload);
        }
        (*(struct PlayerPendingNotification **)&gPlayerPendingActor) = 0;
        gPlayerNotificationPending = 0;
    }
    Player_StopActionSounds(p);
    p->directionFrame = 0;
    p->collider.flags |= 4;
    p->positionResetValue = 0;
    if (p->cameraOffsetActive != 0)
        Player_RestoreCameraFollow(p);
    Player_AddControlFlags(p, 8);
    {
        u32 flag = 0x40000;
        p->statusFlags |= flag;
    }
}
