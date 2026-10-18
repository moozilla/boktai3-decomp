#include "global.h"
#include "player.h"
s32 AreaEffect_IsLinkPlayerCovered(struct Player *);
void Player_ResetActionState(struct Player *p)
{
    p->statusFlags = 1;
    if (AreaEffect_IsLinkPlayerCovered(p)) {
        u32 mask = 0x10;
        p->statusFlags |= mask;
    }
    p->resetValue6F8 = 0;
    p->resetValue6FA = 0;
    p->resetValue6FC = 0;
    p->resetValue41A = 0;
    p->resetFlags &= 0xfd;
}
