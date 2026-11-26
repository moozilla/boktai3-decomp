#include "global.h"
#include "player/player_slot_status.h"

struct P0823CFDC {
    u8 f0[0x20];
    u32 f20;
    u8 f24[0x428 - 0x24];
    u16 f428;
    u16 f42A;
};

s32 Player_IsFlaggedBelowResourceLimit(struct P0823CFDC *p)
{
    if (Player_ReadStatusBits(&p->f20, 0x10) != 0 && p->f428 < p->f42A)
        return 1;
    return 0;
}
