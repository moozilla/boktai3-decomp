#include "global.h"
#include "player/player_slot_status.h"
#include "bosses/ratatosk/ratatosk.h"

extern void *const gUnk_08613F48[];
s32 Ratatosk_ProcessDefeatState(struct Ratatosk *);
s32 Callback_InvokeActor(void *, void *);

void Ratatosk_UpdateCombatMode(struct Ratatosk *p)
{
    if (!PlayerSlot_ReadStatus(0, 4)) {
        if (Ratatosk_ProcessDefeatState(p) == 0)
            Callback_InvokeActor(p, gUnk_08613F48[p->actionIndex]);
    }
}
