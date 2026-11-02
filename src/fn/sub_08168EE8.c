#include "global.h"
#include "scene_hud_display.h"

s32 PauseMenu_GetEquipmentRowY(struct SceneHudState *, s32, s32, s32, s32);

void SceneHud_TickWidgetSlides(struct SceneHudState *hud)
{
    s32 row;

    if (hud->primarySlideTimer) {
        hud->primarySlideTimer--;
        row = 0;
        do {
            hud->primaryWidgets[row].position.y = PauseMenu_GetEquipmentRowY(
                hud, row, hud->primarySelection, 3, hud->primarySlideTimer);
            row++;
        } while (row < 3);
    }

    if (hud->secondarySlideTimer) {
        hud->secondarySlideTimer--;
        row = 0;
        do {
            hud->secondaryWidgets[row].position.y =
                PauseMenu_GetEquipmentRowY(hud, row, hud->selection, 3, hud->secondarySlideTimer);
            row++;
        } while (row < 3);
    }

    if (hud->tertiarySlideTimer) {
        hud->tertiarySlideTimer--;
        row = 0;
        do {
            hud->tertiaryWidgets[row].position.y = PauseMenu_GetEquipmentRowY(
                hud, row, hud->tertiarySelection, 3, hud->tertiarySlideTimer);
            row++;
        } while (row < 3);
    }
}
