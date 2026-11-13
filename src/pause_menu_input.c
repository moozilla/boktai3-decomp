#include "pause_menu.h"
#include "message_window.h"
#include "scrolling_dialogue.h"
#include "player_cue_sequence.h"

u32 PauseMenu_ReadDirectionRepeat(struct SceneHudState *hud)
{
    u32 pressed = gPadState[1] & 0xf0;
    if (pressed) {
        hud->directionRepeatTimer = 0;
        return gPadState[1];
    }
    if (gPadState[0] & 0xf0) {
        if (++hud->directionRepeatTimer > 9) {
            hud->directionRepeatTimer = pressed;
            return gPadState[0];
        }
    }
    return 0;
}

void PauseMenu_SetConfirmationIndicator(struct SceneHudState *hud, s32 choice)
{
    s32 keyframe;
    PauseMenu_ClearSpriteFlags(hud, 7, 1);
    PauseMenu_SetSpritePosition(hud, 7, 0, 0);
    hud->binaryChoice = choice;
    if ((u8)choice)
        keyframe = 0x4b;
    else
        keyframe = 0x4c;
    PauseMenu_LoadSpriteKeyframe(hud, 7, keyframe);
}

s32 PauseMenu_UpdateBinaryChoice(struct SceneHudState *hud)
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

void PauseMenu_HideAllSprites(struct SceneHudState *hud)
{
    u32 hidden = 1;
    s32 i;
    u32 *flags = &hud->categoryWidgets[0].flags;
    struct SceneInputWidget *widget;
    for (i = 0x7b; i >= 0; i--) {
        *flags |= hidden;
        flags += sizeof(struct SceneInputWidget) / sizeof(*flags);
    }
    hud->selectorFlags |= 1;
    widget = &hud->comparisonUnlockWidget;
    widget->flags |= 1;
    widget = &hud->comparisonActionWidget;
    widget->flags |= 1;
    hud->promptSprite.flags |= 1;
}
