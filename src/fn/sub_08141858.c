#define Player_UpdateCameraInput            sub_08141858
#define Player_CanUseCameraAction           sub_08141800
#define Player_BeginDirectionalCameraOffset sub_08141738
#define Player_UpdateCameraOffset           sub_0814159C
#define Player_EndCameraOffset Player_RestoreCameraFollow
#include "global.h"
#include "player.h"
s32 sub_08141800(struct Player *, s32);
void sub_08141738(struct Player *);
void sub_0814159C(struct Player *);
void Player_RestoreCameraFollow(struct Player *);
void sub_08141858(struct Player *p, s32 action)
{
    if ((*p->inputKeys & 0x100) && sub_08141800(p, action)) {
        if ((p->actionMode == 0 || p->actionMode == 2 || p->actionMode == 1 ||
             p->actionMode == 6) &&
            p->tetherMode == 4)
            sub_08141738(p);
        else
            sub_0814159C(p);
    } else {
        if (p->cameraOffsetActive != 0)
            Player_RestoreCameraFollow(p);
    }
    p->cameraInputProcessed = 1;
}
