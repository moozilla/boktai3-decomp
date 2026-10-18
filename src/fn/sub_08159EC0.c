#define Player_ResetModePosition sub_08159EC0
#include "global.h"
#include "player.h"
void Player_SetActionPhase(struct Player *, u8);
s32 sub_08159EC0(struct Player *p, const struct SceneVec *position, s32 value)
{
    if (p->kind == 1 && p->tetherMode == 0x1A) {
        p->position = *position;
        Player_SetActionPhase(p, 2);
        p->positionResetValue = value;
        p->positionResetTimer = 0;
        p->positionResetCounter = 0;
        p->positionResetState = 0;
        return 0;
    }
    return -1;
}
