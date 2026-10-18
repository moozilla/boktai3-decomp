#define ScenePanel_StopSound Vanargand_StopActionSequence
#include "global.h"
/* Tentative ScenePanel family; names describe observed state and sprite operations. */
struct ScenePanelActiveTransition {
    u8 pad0[0xAA];
    u8 state;
    u8 padAB[3];
    u8 nextState;
    u8 active, pending;
    u8 padB1[0xC4 - 0xB1];
    u32 counter;
};
void Vanargand_StopActionSequence(void *);
static inline u8 ScenePanel_ConsumePending(struct ScenePanelActiveTransition *panel)
{
    u8 *pending = &panel->pending;
    if (*pending != 0) {
        *pending = 0;
        *(pending - 1) = 0;
        return TRUE;
    }
    return FALSE;
}
void ScenePanel_UpdateActiveTransition(struct ScenePanelActiveTransition *panel)
{
    if (ScenePanel_ConsumePending(panel)) {
        u32 state = 14;
        Vanargand_StopActionSequence((u8 *)panel + 0x21C);
        panel->state = state;
    }
    if (panel->active != 0) {
        u32 nextState = 3;
        panel->nextState = nextState;
        panel->counter = 0;
        panel->pending = 1;
    }
    panel->counter++;
}
