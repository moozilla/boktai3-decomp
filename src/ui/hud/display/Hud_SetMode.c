#include "global.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "system/game_state/game_state.h"
#include "ui/pause/scene_hud_display.h"
void Hud_SetElementGroupFlags(struct SceneHudState *);
void Hud_HideAuxiliaryElements(struct SceneHudState *);
void Bg_ClearTilemap(s32);
void Hud_SetupBackgroundLayer(struct SceneHudState *, u32, s32, s32, u32);
void SceneHud_RefreshWidgetVisibility(struct SceneHudState *);
void MessageWindow_SetPalette(s32);
void PauseMenu_RefreshStatusPanel(struct SceneHudState *);
/* Records the HUD control mode and applies it unless the HUD is still
   initializing. Modes three and up hide the field widgets; mode eight shows
   the participant and time widgets again. */
void Hud_SetMode(u32 mode)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    s32 i;
    struct AnimSprite *sprite;
    if (!hud)
        return;
    hud->controlMode = mode;
    if (hud->initializationState)
        return;
    if ((u8)mode == 7) {
        Hud_SetElementGroupFlags(hud);
        AnimSprite_Hide((struct AnimSprite *)&hud->controlAnimation);
        AnimSprite_Hide(&hud->countdownDigits[0]);
        AnimSprite_Hide(&hud->countdownDigits[1]);
        AnimSprite_Hide(&hud->countdownDigits[2]);
        hud->pageTransitions[1] = 0;
        Hud_HideAuxiliaryElements(hud);
        Bg_ClearTilemap(0);
    } else if ((u8)mode == 8) {
        Hud_SetElementGroupFlags(hud);
        hud->controlAnimation.flags |= 1;
        hud->countdownDigits[0].flags |= 1;
        hud->countdownDigits[1].flags |= 1;
        hud->countdownDigits[2].flags |= 1;
        hud->pageTransitions[1] = 0;
        for (i = 0, sprite = hud->fieldWidgets.meterWidgets; i < 2; sprite++, i++)
            AnimSprite_Show(sprite);
        for (i = 0, sprite = hud->fieldWidgets.timeDigits; i < 6; sprite++, i++)
            AnimSprite_Show(sprite);
        Bg_ClearTilemap(0);
        Hud_SetupBackgroundLayer(hud, hud->largePanelResource, 0, 0, hud->largePanelConfig);
    } else if ((u8)mode == 9) {
        Hud_SetupBackgroundLayer(hud, hud->panelResource, 0, 2, hud->panelConfig);
    } else {
        s32 shown = 0;
        s32 row;
        if ((u8)mode <= 2) {
            if (!(gEngineFlags & 0x1000))
                SceneHud_RefreshWidgetVisibility(hud);
            if (hud->controlMode == 1)
                hud->storedEnergy = gGameState->unk618;
            row = 0;
        } else {
            Hud_SetElementGroupFlags(hud);
            hud->controlAnimation.flags |= 1;
            hud->countdownDigits[0].flags |= 1;
            hud->countdownDigits[1].flags |= 1;
            hud->countdownDigits[2].flags |= 1;
            row = hud->controlMode - 2;
            shown = 1;
            hud->pageTransitions[1] = 0;
            Hud_HideAuxiliaryElements(hud);
        }
        Hud_SetupBackgroundLayer(hud, hud->panelResource, 0, row, hud->panelConfig);
        if (shown)
            MessageWindow_SetPalette(0xEFDA);
        if (gEngineFlags & 0x1000)
            PauseMenu_RefreshStatusPanel(hud);
    }
    hud->unknownA8E[0] = 0;
}
