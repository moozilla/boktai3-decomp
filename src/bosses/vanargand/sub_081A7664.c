#include "global.h"
/* Tentative ScenePanel family; names describe observed state and sprite operations. */

struct ScenePanelInput {
    u8 pad0[0xAA];
    u8 timer;
    u8 padAB[4];
    u8 active;
    u8 pending;
};
struct ScenePanelSelection {
    u8 pad0[0xF28];
    u32 scripts[3];
    s32 selection;
};
void Script_Call(u32, void *);
static inline u8 ScenePanel_ConsumePending(struct ScenePanelInput *panel)
{
    if (panel->pending != 0) {
        panel->pending = 0;
        panel->active = 0;
        return TRUE;
    }
    return FALSE;
}
void ScenePanel_RunSelectionScript(struct ScenePanelSelection *panel)
{
    struct ScenePanelInput *input = (struct ScenePanelInput *)panel;
    if (ScenePanel_ConsumePending(input)) {
        u8 timer = 30;
        input->timer = timer;
    }
    if (input->active) {
        Script_Call(panel->scripts[panel->selection], 0);
        if (panel->selection <= 2)
            panel->selection++;
    }
}
