#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"

#ifdef NONMATCHING
// fuzzy: 87.2%
// foundry draft (worker best 99.2%); what differs is mostly register allocation and instruction order
#define SceneHud_CyclePrimarySecondaryOptions SceneHud_CyclePrimarySecondaryOptions
#define SceneHud_RefreshPrimaryOptions        SelectionPanel_RefreshVisibleEntries
#define SceneHud_RefreshSecondaryOptions      sub_0816902C
#define SceneHud_TickWidgetSlides             sub_08168EE8
#define gSceneHudInput                        gUnk_03005164
extern u16 gUnk_03005164[];
void SelectionPanel_RefreshVisibleEntries(struct SceneHudState *);
void sub_0816902C(struct SceneHudState *);
void sub_08168EE8(struct SceneHudState *);
void SceneHud_CyclePrimarySecondaryOptions(struct SceneHudState *p)
{
    if ((gUnk_03005260[1] & 0x200) && !p->primarySlideTimer && p->primaryVisible > 1) {
        Sound_PlaySong(0x10B);
        if (++p->primarySelection >= p->primaryVisible)
            p->primarySelection = 0;
        *p->primarySelectionOutput = p->primaryOptions[p->primarySelection].value;
        p->primarySlideTimer = 8;
        SelectionPanel_RefreshVisibleEntries(p);
    }
    if ((gUnk_03005260[1] & 0x100) && !p->secondarySlideTimer && p->secondaryCount > 1) {
        Sound_PlaySong(0x10B);
        if (++p->selection >= p->secondaryCount)
            p->selection = 0;
        {
            u16 *out = &gUnk_02000710->unk50;
            *out = ((struct SceneHudState *)(p->selection * 8 + (u32)p))->secondaryFirstValue;
        }
        p->secondarySlideTimer = 8;
        sub_0816902C(p);
    }
    sub_08168EE8(p);
    Palette_UpdateHighlightPulse(gUnk_03005164, p->unknown13F0);
}
#else
INCLUDE_ASM("asm/nonmatching", SceneHud_CyclePrimarySecondaryOptions);
#endif
