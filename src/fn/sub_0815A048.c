#include "global.h"
#include "ram.h"
#include "player.h"
void Player_AddControlFlags(void *, u32);
void Sound_PlaySong(u32);
// confidence=low: forced variant clears movement state before starting or refreshing stun.
#define Player_TryStartStun sub_0815A048
s32 Player_TryStartStun(struct Player *p, s32 duration, s32 force)
{
    if (force == 1) {
        s32 guard = p->positionResetValue;
        if (guard > 0)
            return 0;
        Player_AddControlFlags(p, 8);
        p->collider.flags |= 4;
        p->directionFrame = 0;
        p->contactSpecial = 0;
        p->positionResetTimer = 0;
        p->positionResetCounter = 0;
        p->positionResetState = 0;
        p->particleEmissionCount = 0;
        gPlayerPendingActor = 0;
        gPlayerNotificationPending = 0;
    }
    {
        u32 kind = p->kind;
        if (kind == 1 && p->tetherMode != 0x1A) {
            if (!p->positionResetTimer) {
                p->positionResetTimer = duration;
                p->positionResetRequested = kind;
                p->positionResetCounter = kind;
                Sound_PlaySong(0xE6);
                return 1;
            }
            p->positionResetRequested = kind;
        }
    }
    return 0;
}
