#include "challenge_menu.h"

s32 GameState_GetPackedTripletValue(s32, s32);
s32 ChallengeMenu_GetRankKeyframe(s32);
void Palette_UpdateHighlightPulse(u16 *, u8 *);

static inline void ShowWidget(struct SceneInputWidget *widget)
{
    widget->flags &= ~1;
}

static inline void HideWidget(struct SceneInputWidget *widget)
{
    widget->flags |= 1;
}

s32 sub_081C58BC(struct ChallengeMenu *menu)
{
    s32 *selection = &menu->selection;
    struct ContactPosition *position;
    s32 rank;

    if (*selection == 0) {
        Anim_LoadKeyframe(&menu->widgets[1], &menu->resources[1], 0x55, 0);
        position = &menu->widgets[1].position;
        position->x = 0;
        position->y = 0;
    } else if (*selection <= 3) {
        Anim_LoadKeyframe(&menu->widgets[1], &menu->resources[1], 0x56, 0);
        position = &menu->widgets[1].position;
        position->x = 0;
        position->y = (*selection - 1) << 4;
    }
    Palette_UpdateHighlightPulse(menu->highlightPalette, &menu->highlightPhase);
    ShowWidget(&menu->widgets[3]);
    if (menu->selection == 0) {
        HideWidget(&menu->widgets[2]);
    } else if (menu->selection <= 3) {
        rank = ChallengeMenu_GetRankKeyframe(
            GameState_GetPackedTripletValue(menu->group, menu->selection - 1));
        if (rank >= 0) {
            Anim_LoadKeyframe(&menu->widgets[2], &menu->resources[1], (u16)rank, 0);
            ShowWidget(&menu->widgets[2]);
        } else {
            HideWidget(&menu->widgets[2]);
        }
    }
}
