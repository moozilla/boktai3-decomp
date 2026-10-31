#include "player_cue.h"
s32 Collider_Refresh(void *);
void Player_UpdateColliderRefreshCountdown(struct Player *p)
{
    u8 *remaining = &p->cueColliderPhase;
    if (*remaining != 0) {
        Collider_Refresh(&p->cueCollider);
        *remaining = *remaining - 1;
    }
}
