#define SelectionMenu_ApplyConfirmedReward sub_0816B250
#define SelectionMenu_SetPhase Hud_SetUpdateCallback
#define SelectionMenu_ReadConfirmation     sub_08165A30
#define gGameState                         gUnk_02000710
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
s32 sub_08165A30(struct SelectionMenu *);
void Hud_SetUpdateCallback(struct SelectionMenu *, void (*)(struct SelectionMenu *));
void sub_0816B0AC(struct SelectionMenu *);
void sub_0816AE38(struct SelectionMenu *);
void sub_0816B250(struct SelectionMenu *p)
{
    s32 result;
    Anim_Step(p->primaryAnimation, p->animationData);
    result = sub_08165A30(p);
    if (result == 0) {
        Sound_PlaySong(0xDE);
        MessageWindow_ShowMessage(7);
        Hud_SetUpdateCallback(p, sub_0816B0AC);
    } else if (result == 1) {
        Sound_PlaySong(0xDD);
        ((struct GameState *)(u8 *)gUnk_02000710)->menuRewardBalance += (p->count - 1) * 50;
        if (((struct GameState *)(u8 *)gUnk_02000710)->menuRewardBalance > 9999)
            (*(struct GameState *volatile *)&gUnk_02000710)->menuRewardBalance = 9999;
        p->confirmed = result;
        p->resultMode = 2;
        sub_0816AE38(p);
    }
}
