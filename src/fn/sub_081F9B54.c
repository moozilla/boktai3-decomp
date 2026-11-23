#include "global.h"
#include "player_slot_status.h"
#include "ratatosk.h"

extern void *const gUnk_08613F48[];
s32 sub_081F79F8(struct Ratatosk *);
s32 Callback_InvokeActor(void *, void *);

void Ratatosk_UpdateCombatMode(struct Ratatosk *p)
{
    if (!PlayerSlot_ReadStatus(0, 4)) {
        if (sub_081F79F8(p) == 0)
            Callback_InvokeActor(p, gUnk_08613F48[p->actionIndex]);
    }
}
