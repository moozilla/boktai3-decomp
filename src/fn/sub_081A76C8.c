#include "global.h"

// confidence=low: enters state 29 and consumes the pending transition trigger.
#define Vanargand_BeginExitTransition sub_081A76C8
/* Tentative ScenePanel family; names describe observed state and sprite operations. */
struct ScenePanelTimedExit {
    u8 pad0[0xAA];
    u8 state;
    u8 padAB[4];
    u8 active, pending;
    u8 padB1[0xC4 - 0xB1];
    u32 counter;
};
void Vanargand_StopActionSequence(void *);
u32 Trigger_ConsumePending(void *, u32);
static inline u8 ScenePanel_ConsumePending(struct ScenePanelTimedExit *panel)
{
    if (panel->pending) {
        panel->pending = 0;
        panel->active = 0;
        return TRUE;
    }
    return FALSE;
}
void Vanargand_BeginExitTransition(struct ScenePanelTimedExit *panel)
{
    if (ScenePanel_ConsumePending(panel)) {
        u32 state = 29;
        Vanargand_StopActionSequence((u8 *)panel + 0x21C);
        panel->state = state;
        Trigger_ConsumePending((u8 *)panel + 0x118, 1);
    }
}
