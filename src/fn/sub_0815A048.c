// MATCHING_CONCESSION: register-pinning
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
        {
            u16 *flags = &p->collider.flags;
            register u32 old asm("r3");
            u32 bits;
            u32 zeroByte, zeroHalf;
            old = *flags;
            bits = 4;
            p->collider.flags = bits | old;
            zeroHalf = 0;
            p->directionFrame = zeroHalf;
            zeroByte = 0;
            p->contactSpecial = zeroByte;
            p->positionResetTimer = zeroHalf;
            p->positionResetCounter = zeroHalf;
            p->positionResetState = zeroByte;
            p->particleEmissionCount = zeroHalf;
            gPlayerPendingActor = zeroHalf;
            gPlayerNotificationPending = zeroHalf;
        }
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
