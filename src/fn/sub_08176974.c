#include "global.h"
#include "scene_hud_display.h"
void Hud_InitSelectionBounds(u32, u32);
void SceneHud_DrawSelectedClock(void *);
void PauseMenu_UpdateHighlightPalette(void *);
void PauseMenu_LoadSpriteKeyframe(void *, int, int);
s32 Callback_InvokeActor(void *, void *);
void Sound_PlaySong(u32);
void Hud_SetUpdateCallback(void *, void (*)(void *));
void SceneHud_FadeOutPage(void *);
u8 SceneDialog_FindNextEnabledEntry(void *, u32);
u8 SceneDialog_FindPreviousEnabledEntry(void *, u32);
void SceneDialog_ConfigureVariantSlot(void *);
void SceneHud_UpdatePageInput(struct SceneHudState *p)
{
    u32 direction;
    Hud_InitSelectionBounds(0x40, 0x1084);
    SceneHud_DrawSelectedClock(p);
    PauseMenu_UpdateHighlightPalette(p);
    if (p->statusRefreshRequested)
        goto close;
    if (p->comparisonModifier) {
        PauseMenu_LoadSpriteKeyframe(p, 1, 0x43);
        PauseMenu_LoadSpriteKeyframe(p, 2, 0x44);
        Callback_InvokeActor(p, p->pageCallback);
        return;
    }
    PauseMenu_LoadSpriteKeyframe(p, 1, 0x41);
    PauseMenu_LoadSpriteKeyframe(p, 2, 0x42);
    if (gPadState[1] & 2) {
        Sound_PlaySong(0x119);
    close:
        Hud_SetUpdateCallback(p, SceneHud_FadeOutPage);
        return;
    }
    if (!p->pageInputCooldown) {
        direction = 0;
        if (gPadState[1] & 0x100) {
            p->pageRepeatTimer = 0;
            direction = 1;
        } else if (gPadState[1] & 0x200) {
            p->pageRepeatTimer = 0;
            direction = 2;
        } else if (gPadState[0] & 0x100) {
            if (++p->pageRepeatTimer > 9) {
                p->pageRepeatTimer = 0;
                direction = 1;
            }
        } else if (gPadState[0] & 0x200) {
            if (++p->pageRepeatTimer > 9) {
                p->pageRepeatTimer = 0;
                direction = 2;
            }
        }
        if (direction == 1) {
            Sound_PlaySong(0x10A);
            p->mode = SceneDialog_FindNextEnabledEntry(p, p->mode);
        } else if (direction == 2) {
            Sound_PlaySong(0x10A);
            p->mode = SceneDialog_FindPreviousEnabledEntry(p, p->mode);
        } else
            goto callback;
        SceneDialog_ConfigureVariantSlot(p);
        return;
    } else
        --p->pageInputCooldown;
callback:
    Callback_InvokeActor(p, p->pageCallback);
}
