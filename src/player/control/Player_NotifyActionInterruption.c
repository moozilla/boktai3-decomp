#include "global.h"
#include "engine/ram.h"
#include "player/player.h"
#include "player/cutscene/player_cue_commands.h"

void sub_08249244(void *, s32, s32, void *);

void Player_NotifyActionInterruption(struct Player *p)
{
    struct PlayerPendingNotification *notification = gPlayerPendingActor;
    if (notification != NULL && p->tetherPhase == 2 && p->tetherMode == 12) {
        if (notification->flags & 0x10)
            sub_08249244(notification->receiver, 1, 60, notification->callback);
        else
            sub_08249244(notification->receiver, 1, 60, notification->callback);
    }
}
