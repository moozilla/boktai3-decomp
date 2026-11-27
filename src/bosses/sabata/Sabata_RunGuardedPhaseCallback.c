#include "global.h"
#include "player/player_slot_status.h"
#include "npc/story_character/state_actor.h"

extern const u32 gUnk_08611CB8[];
s32 SabataState_BeginDepletedState(void *);
void Callback_InvokeActor(void *, u32);

void Sabata_RunGuardedPhaseCallback(struct StateActor *p)
{
    if (!PlayerSlot_ReadStatus(0, 4)) {
        if (SabataState_BeginDepletedState(p) == 0)
            Callback_InvokeActor(p, gUnk_08611CB8[p->actionIndex]);
    }
}
