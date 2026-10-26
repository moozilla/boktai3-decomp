#define SelectionMenu_SetPhase Hud_SetUpdateCallback
#include "global.h"
#include "game_state.h"
struct SelectionMenu {
    u8 unknown0[0xC4];
    u8 animationData[0x3B1C];
    u8 primaryAnimation[0xC94];
    u8 confirmed;
    u8 resultMode;
    u8 unknown4876[0x70];
    u8 count;
};
s32 Anim_Step(void *, void *);
void Sound_PlaySong(u32);
s32 MessageWindow_ShowMessage(s32);
s32 SceneHud_UpdateBinaryChoice(struct SelectionMenu *);
void Hud_SetUpdateCallback(struct SelectionMenu *, void (*)(struct SelectionMenu *));
void SelectionMenu_UpdateConfirmInput(struct SelectionMenu *);
void ScenePanel_BeginAccountExit(struct SelectionMenu *);
void SelectionMenu_ApplyConfirmedReward(struct SelectionMenu *p)
{
    s32 result;
    Anim_Step(p->primaryAnimation, p->animationData);
    result = SceneHud_UpdateBinaryChoice(p);
    if (result == 0) {
        Sound_PlaySong(0xDE);
        MessageWindow_ShowMessage(7);
        Hud_SetUpdateCallback(p, SelectionMenu_UpdateConfirmInput);
    } else if (result == 1) {
        Sound_PlaySong(0xDD);
        ((struct GameState *)(u8 *)gGameState)->menuRewardBalance += (p->count - 1) * 50;
        if (((struct GameState *)(u8 *)gGameState)->menuRewardBalance > 9999)
            (*(struct GameState *volatile *)&gGameState)->menuRewardBalance = 9999;
        p->confirmed = result;
        p->resultMode = 2;
        ScenePanel_BeginAccountExit(p);
    }
}
