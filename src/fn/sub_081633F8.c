#include "global.h"
#include "engine.h"
#include "game_state.h"
#define TransitionPanel_UpdateAnimation GameOver_UpdateLetterAnimation
struct TransitionPanelOverlay {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x54];
};
struct TransitionPanel {
    u8 unknown0[0x2B8];
    struct TransitionPanelOverlay overlay;
    u8 unknown318[0x20];
    const void *text;
    u8 unknown33C[4];
    u8 closeState, state;
    u8 unknown342[6];
    u16 costs[2];
    u8 selection;
};
void GameOver_UpdateLetterAnimation(struct TransitionPanel *);
void Hud_SetMode(s32);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetNumberArgument(s32, s32);
s32 MessageWindow_ShowMessage(s32);
void TransitionPanel_UpdateCostPrompt(struct TransitionPanel *p)
{
    GameOver_UpdateLetterAnimation(p);
    if (p->state == 6) {
        p->overlay.flags &= ~1;
        Hud_SetMode(3);
        MessageWindow_SetBounds(1, 13, 28, 4);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetMessageTable(p->text);
        MessageWindow_SetNumberArgument(0, p->costs[p->selection]);
        if (gGameState->unk7B0 >= p->costs[p->selection])
            MessageWindow_ShowMessage(0);
        else
            MessageWindow_ShowMessage(1);
        p->closeState = 3;
    }
}
