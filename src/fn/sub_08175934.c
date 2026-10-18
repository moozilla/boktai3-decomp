#define SelectionPanel_CaptureSettings sub_08175934
#define SelectionPanel_GetMappedSetting sub_08175874
#define gPanelGameState gUnk_02000710
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
#include "game_state.h"

struct SelectionPanel {
    u8 unk0[0x48C0];
    u8 state, setting10, selection, setting14;
};
u32 sub_08175874(void);
void sub_08175934(struct SelectionPanel *p)
{
    p->state = 0;
    p->setting10 = gUnk_02000710->values08[4];
    p->selection = sub_08175874();
    p->setting14 = gUnk_02000710->unk14;
}
