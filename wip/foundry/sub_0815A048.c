#define Player_RequestPositionReset sub_0815A048
#define gPendingPlayerNotification  gUnk_0200054C
#define gPlayerNotificationPending  gUnk_02000594
#include "global.h"
#include "player.h"

void sub_0813B134(struct Player *, s32);
void Sound_PlaySong(u32);
extern u32 gUnk_0200054C;
extern u16 gUnk_02000594;

s32 sub_0815A048(struct Player *p, s32 duration, s32 reset)
{
    if (reset == 1) {
        if ((s32)(u32)p->positionResetValue > 0)
            return 0;
        sub_0813B134(p, 8);
        {
            u8 zero = 0;
            u16 zero16 = 0;
            p->collider.flags |= 4;
            p->directionFrame = zero16;
            p->contactSpecial = zero;
            p->positionResetTimer = zero16;
            p->positionResetCounter = zero16;
            p->positionResetState = zero;
            p->particleEmissionCount = zero16;
            gUnk_0200054C = 0;
            gUnk_02000594 = zero16;
        }
    }
    if (p->kind == 1 && p->tetherMode != 0x1A) {
        if (p->positionResetTimer == 0) {
            p->positionResetTimer = duration;
            p->positionResetRequested = 1;
            p->positionResetCounter = 1;
            Sound_PlaySong(0xE6);
            return 1;
        }
        p->positionResetRequested = 1;
    }
    return 0;
}
