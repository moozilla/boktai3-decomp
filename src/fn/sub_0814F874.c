#include "player_cue.h"

void PlayerCue_AcknowledgePendingTrigger(struct Player *p)
{
    u8 *pending = &PlayerCue_Messages(p)->acknowledgementPending;
    if (*pending != 0) {
        Trigger_ConsumePending(&PlayerCue_Messages(p)->receiver, 1);
        *pending = 0;
    }
}
