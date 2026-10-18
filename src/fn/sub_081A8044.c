#define ScenePanel_StopSound Vanargand_StopActionSequence
#include "global.h"
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
void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);
static inline u8 ScenePanel_ConsumePending(struct ScenePanelTimedExit *panel)
{
    if (panel->pending) {
        panel->pending = 0;
        panel->active = 0;
        return TRUE;
    }
    return FALSE;
}
void ScenePanel_UpdateDelayedPairState(void *object)
{
    struct ScenePanelTimedExit *panel = object;
    if (ScenePanel_ConsumePending(panel)) {
        u32 state = 1;
        Vanargand_StopActionSequence((u8 *)panel + 0x21C);
        panel->state = state;
    }
    if (panel->counter > 128) {
        ScenePanel_UpdatePairConditions(object);
        ScenePanel_ApplyPairTransition(object);
    }
    panel->counter++;
}
