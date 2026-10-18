#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
s32 PlayerContact_ApplySyntheticContact(struct Player *, s32, u32, u32, u32, u32);
static inline void lockPositionRequests(u32 *flags, u32 mask)
{
    *flags |= mask;
}
s32 Player_ApplyForcedFacing(struct Player *p, s32 frame, u32 direction, s32 resetValue,
                             s32 request, u32 option)
{
    if (p->tetherMode == 0x1A)
        goto noRequest;
    if (resetValue > 0)
        p->positionResetValue = resetValue;
    direction = ((s32)((direction + 0xE0) & 0xFF) >> 6) * 2 + 1;
    p->facing = direction;
    if (direction > 4) {
        p->heading = 8 - direction;
        p->side = 1;
    } else {
        p->heading = direction;
        p->side = 0;
    }
    p->directionFrame = frame;
    p->contactSpecial = 1;
    if (p->tetherMode != 0x1E)
        Sound_PlaySong(0xE6);
    if (p->positionResetTimer)
        p->positionResetTimer = 0;
    lockPositionRequests(&p->statusFlags, 0x40000);
    if (request > 0)
        goto startRequest;
noRequest:
    return 0;
startRequest:
    return PlayerContact_ApplySyntheticContact(p, request, 0, 0, 0, option);
}
