#include "global.h"
#include "player.h"
#define TransitionPanel_TryBeginClose sub_08163310
#define TransitionPanel_HideSprites GameOver_ShowGraphics
struct TransitionPanel {
    u8 unknown0[0x340];
    u8 closeState;
    u8 unknown341[0xB];
    u8 deferClose;
};
extern u32 gUnk_030053F4, gUnk_030053F0;
void GameOver_ShowGraphics(struct TransitionPanel *);
void Hud_SetMode(s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void sub_08163310(struct TransitionPanel *p)
{
    u32 mask = 0x400;
    if ((gUnk_030053F4 | gUnk_030053F0) & mask) {
        GameOver_ShowGraphics(p);
        Hud_SetMode(7);
        if ((u8)(gUnk_02000580[0]->tetherMode - 0x21) <= 1) {
            p->deferClose = 1;
        } else {
            ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0x1FFF, 2);
        }
        p->closeState = 1;
    }
}
