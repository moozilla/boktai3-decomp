#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

extern void (*const gUnk_08613E14[])(void);

u32 Ratatosk_InitRequestedState(struct Ratatosk *p)
{
    if ((u8)(p->actionAnimation - 1) > 0x24)
        return FALSE;
    Ratatosk_StartReaction(p, gUnk_08613E14[p->actionAnimation], p->actionAnimation);
    return TRUE;
}
