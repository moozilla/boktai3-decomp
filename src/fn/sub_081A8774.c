#define ScenePanel_UpdatePairConditions sub_081A7C94
#define ScenePanel_ApplyPairTransition sub_081A7D80
#define ScenePanel_ResetPairMode sub_081A8774
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
void sub_081A7C94(void *);
void sub_081A7D80(void *);
static inline u8 ScenePanel_ConsumePending(struct ScenePanelTimedExit *panel)
{
    if (panel->pending) {
        panel->pending = 0;
        panel->active = 0;
        return TRUE;
    }
    return FALSE;
}
struct ScenePanelPairMode {
    u8 pad0[0x5C2];
    s16 mode;
};
void sub_081A8774(struct ScenePanelPairMode *object)
{
    struct ScenePanelTimedExit *panel = (struct ScenePanelTimedExit *)object;
    if (ScenePanel_ConsumePending(panel)) {
        u32 state = 16;
        Vanargand_StopActionSequence((u8 *)panel + 0x21C);
        panel->state = state;
    }
    if (panel->active != 0) {
        object->mode = 0;
        sub_081A7C94(object);
        sub_081A7D80(object);
    }
}
