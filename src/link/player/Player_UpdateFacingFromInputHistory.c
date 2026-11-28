#include "global.h"
#include "player/player.h"

u8 Player_GetLinkedSmoothedDirection(void *);
void Player_UpdateActionFacingSide(void *);

s32 Player_UpdateFacingFromInputHistory(struct Player *p)
{
    s32 updated = 0;
    if ((s16)p->directionHistory[0] != -1 && p->cameraOffsetActive == 0) {
        p->facing = Player_GetLinkedSmoothedDirection(p);
        updated = 1;
    }
    Player_UpdateActionFacingSide(p);
    return updated;
}
