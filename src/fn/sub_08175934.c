#define gPanelGameState gGameState
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
#include "game_state.h"

struct SelectionPanel {
    u8 unk0[0x48C0];
    u8 state, setting10, selection, setting14;
};
u32 SceneCatalog_GetMappedState(void);
void SelectionPanel_CaptureSettings(struct SelectionPanel *p)
{
    p->state = 0;
    p->setting10 = gGameState->values08[4];
    p->selection = SceneCatalog_GetMappedState();
    p->setting14 = gGameState->unk14;
}
