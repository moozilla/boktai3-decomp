#include "engine_globals.h"
#include "game_state.h"
#include "player_core.h"
#include "ram.h"
#include "runtime_input.h"

/* tentative: reads the action mode from the script and installs its callback and input slot */

extern void *const gUnk_08610964[];

void Player_UpdateLinkModeMovement(struct Player *);
void ActorSequence_Begin(struct Player *, u32, u32);
void Player_ClearDirectionHistory(struct Player *);

s32 Player_InitActionMode(struct Player *p)
{
    s32 i;

    p->kind = 1;
    p->statGaugeFull = 0;
    if (Script_SeekToKeyword('k')) {
        p->actionMode = Script_GetValue();
        if ((u8)(p->actionMode - 1) <= 1)
            p->statGaugeFull = 1;
    } else {
        p->actionMode = gGameState->form;
    }
    if (p->actionMode == 6)
        p->timedEventSlot = 1;
    else
        p->timedEventSlot = 0;
    Player_RecalculateDerivedStats(p);
    if (gEngineFlags & 0x1000) {
        void (*update)(struct Player *) = Player_UpdateLinkModeMovement;
        for (i = 0; i < 2; i++)
            gPlayerInitialSlotValues[i] = 0;
        p->updateCallback = update;
    } else {
        p->updateCallback = gUnk_08610964[p->actionMode];
    }
    p->unk45C = 1;
    ActorSequence_Begin(p, 0, 0);
    p->pendingCallback = NULL;
    p->inputKeys = &gPadState[p->slot].held;
    Player_ClearDirectionHistory(p);
    if (p->slot == 0) {
        p->sceneMode = gPlayerInteractionMode;
        p->unk536 = gPlayerScriptState;
        p->active = gPlayerPassageState;
        for (i = 0; i < 2; i++)
            p->scriptStatus[i] = gPlayerInitialSlotValues[i];
    } else {
        s32 j;
        p->sceneMode = 0;
        p->unk536 = 0;
        p->active = 0;
        for (j = 0; j < 2; j++)
            p->scriptStatus[j] = 0;
    }
    p->catalogSavedOption = gGameState->values08[4];
    if (p->scriptStatus[1] != 0)
        p->catalogSavedOption = gPlayerSavedMenuOption;
    p->actionLatch464 = 0;
    p->contactThresholdReady = 0;
    p->unk432[1] = 0;
    p->historyDirectionMode = 0;
    p->actionRequestRestricted = 0;
    p->unk461[0] = 0;
    return 0;
}
