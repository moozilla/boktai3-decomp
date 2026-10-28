#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
#include "player.h"
#include "render.h"

void ResultPanel_DrawCappedCounters(struct SceneHudState *, s32, s32, s32);
void ResultPanel_DrawCappedSmallCounters(struct SceneHudState *, s32, s32, s32);
void StatDisplay_DrawPlayerValue(struct SceneHudState *, s32, s32, s32);
void PauseMenu_FinishEquipmentSelection(struct SceneHudState *);
s32 PauseMenu_GetSelectedListEntryType(struct SceneHudState *);
void SelectionPanel_RefreshVisibleEntries(struct SceneHudState *);
void SceneHud_RefreshSecondaryOptions(struct SceneHudState *);
void SelectionPanel_RefreshSecondaryEntries(struct SceneHudState *);
void SceneHud_TickWidgetSlides(struct SceneHudState *);

/* View of the stride-eight secondary option records at SceneHudState + 0xDA8. */
struct SceneHudSecondaryView {
    u8 unknown0[0xDA8];
    struct SceneHudSecondaryOption options[16];
};

static inline s32 Player_IsEquipmentLocked(struct Player *owner)
{
    if (gEngineFlags & 0x1000)
        goto zero;
    if ((u8)(owner->actionMode - 1) <= 1)
        goto one;
zero:
    return 0;
one:
    return 1;
}
static inline s32 PauseMenu_IsEquipmentLocked(struct SceneHudState *p)
{
    if (gEngineFlags & 0x1000)
        goto zero;
    if ((u8)(p->actionOwner->actionMode - 1) <= 1)
        goto one;
zero:
    return 0;
one:
    return 1;
}

void PauseMenu_UpdateEquipmentSelection(struct SceneHudState *p)
{
    ResultPanel_DrawCappedCounters(p, 0, 3, 1);
    ResultPanel_DrawCappedSmallCounters(p, 0, 3, 2);
    StatDisplay_DrawPlayerValue(p, 0, 3, 3);
    if (!(gUnk_03005260[0] & 4) && !p->primarySlideTimer && !p->secondarySlideTimer &&
        !p->tertiarySlideTimer) {
        PauseMenu_FinishEquipmentSelection(p);
    } else {
        if ((gUnk_03005260[1] & 0x200) && !p->primarySlideTimer && p->primaryVisible) {
            struct Player *owner = p->actionOwner;
            if (owner->actionRequestBlocked || Player_IsEquipmentLocked(owner)) {
                Sound_PlaySong(0x192);
            } else {
                Sound_PlaySong(0x10B);
                p->primarySelection++;
                if (p->primarySelection >= p->primaryVisible)
                    p->primarySelection = 0;
                *p->primarySelectionOutput = p->primaryOptions[p->primarySelection].value;
                p->primarySlideTimer = 8;
                SelectionPanel_RefreshVisibleEntries(p);
            }
        }
        if ((gUnk_03005260[1] & 0x100) && !p->secondarySlideTimer && p->secondaryCount) {
            if (PauseMenu_IsEquipmentLocked(p)) {
                Sound_PlaySong(0x192);
            } else {
                Sound_PlaySong(0x10B);
                p->selection++;
                if (p->selection >= p->secondaryCount)
                    p->selection = 0;
                {
                    u16 *out = &gGameState->unk50;
                    *out = ((struct SceneHudSecondaryView *)p)->options[p->selection].value;
                }
                p->secondarySlideTimer = 8;
                SceneHud_RefreshSecondaryOptions(p);
            }
        }
        if (PauseMenu_GetSelectedListEntryType(p) && (gUnk_03005260[1] & 2) && p->tertiaryVisible) {
            if (PauseMenu_IsEquipmentLocked(p)) {
                Sound_PlaySong(0x192);
            } else {
                Sound_PlaySong(0x10B);
                p->tertiarySelection++;
                if (p->tertiarySelection >= p->tertiaryVisible)
                    p->tertiarySelection = 0;
                {
                    s16 *out = &gGameState->actionAuxiliarySlot;
                    *out = (s8)p->tertiaryOptions[p->tertiarySelection].slot;
                }
                p->tertiarySlideTimer = 8;
                SelectionPanel_RefreshSecondaryEntries(p);
            }
        }
    }
    SceneHud_TickWidgetSlides(p);
    Palette_UpdateHighlightPulse(gBgPaletteBank13Color2, &p->unknown13F0);
}
