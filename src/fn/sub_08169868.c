#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
#include "render.h"

// foundry draft (worker best 99.2%); what differs is mostly register allocation and instruction order

#define SceneHud_RefreshPrimaryOptions SelectionPanel_RefreshVisibleEntries
void SelectionPanel_RefreshVisibleEntries(struct SceneHudState *);
void SceneHud_RefreshSecondaryOptions(struct SceneHudState *);
void SceneHud_TickWidgetSlides(struct SceneHudState *);
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
            u16 *out = &gGameState->unk50;
            *out = *(s8 *)((u8 *)p + (p->selection << 3) + 0xDA8);
        }
        p->secondarySlideTimer = 8;
        SceneHud_RefreshSecondaryOptions(p);
    }
    SceneHud_TickWidgetSlides(p);
    Palette_UpdateHighlightPulse(gBgPaletteBank13Color2, &p->unknown13F0);
}
