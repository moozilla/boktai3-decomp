#define SceneHud_SetWidgetFlags    PauseMenu_AddSpriteFlags
#define SceneHud_SetWidgetKeyframe PauseMenu_LoadSpriteKeyframe
#include "global.h"
#include "scene_hud_display.h"
void PauseMenu_AddSpriteFlags(void *, s32, s32);
void PauseMenu_LoadSpriteKeyframe(void *, s32, s32);
s32 SceneHud_UpdateBinaryChoice(struct SceneHudState *hud)
{
    u32 changed;
    if (gPadState[1] & 1) {
        PauseMenu_AddSpriteFlags(hud, 7, 1);
        return hud->binaryChoice;
    }
    if (gPadState[1] & 2) {
        PauseMenu_AddSpriteFlags(hud, 7, 1);
        return 0;
    }
    changed = 0;
    if (hud->binaryChoice) {
        u16 pressed = gPadState[1] & 0x10;
        changed = pressed != 0;
    } else {
        if (gPadState[1] & 0x20)
            changed = 1;
    }
    if (changed) {
        Sound_PlaySong(0xDC);
        hud->binaryChoice = 1 - hud->binaryChoice;
        PauseMenu_LoadSpriteKeyframe(hud, 7, 0x4C - hud->binaryChoice);
    }
    return -1;
}
