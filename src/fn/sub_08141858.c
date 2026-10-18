#define Player_EndCameraOffset Player_RestoreCameraFollow
#include "global.h"
#include "player.h"
s32 Player_IsActionAllowed(struct Player *, s32);
void Player_BeginDirectionalCameraOffset(struct Player *);
void Player_UpdateCameraOffset(struct Player *);
void Player_RestoreCameraFollow(struct Player *);
void Player_UpdateCameraInput(struct Player *p, s32 action)
{
    if ((*p->inputKeys & 0x100) && Player_IsActionAllowed(p, action)) {
        if ((p->actionMode == 0 || p->actionMode == 2 || p->actionMode == 1 ||
             p->actionMode == 6) &&
            p->tetherMode == 4)
            Player_BeginDirectionalCameraOffset(p);
        else
            Player_UpdateCameraOffset(p);
    } else {
        if (p->cameraOffsetActive != 0)
            Player_RestoreCameraFollow(p);
    }
    p->cameraInputProcessed = 1;
}
