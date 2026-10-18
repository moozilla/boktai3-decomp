#include "global.h"
#include "scene_hud_display.h"
#define SceneHud_UpdatePageInput    sub_08176974
#define SceneHud_FadeOutPage        sub_08176B00
#define gSceneInputKeys             gUnk_03005260
#define SceneHud_SelectPreviousPage sub_08176838
#define SceneHud_SelectNextPage     sub_08176874
void Hud_InitSelectionBounds(u32, u32);
void sub_081658C8(void *);
void PauseMenu_UpdateHighlightPalette(void *);
void PauseMenu_LoadSpriteKeyframe(void *, int, int);
s32 sub_0824923C(void *, void *);
void Sound_PlaySong(u32);
void Hud_SetUpdateCallback(void *, void (*)(void *));
void sub_08176B00(void *);
u8 sub_08176838(void *, u32);
u8 sub_08176874(void *, u32);
void sub_08176590(void *);
void sub_08176974(struct SceneHudState *p)
{
    u32 direction;
    Hud_InitSelectionBounds(0x40, 0x1084);
    sub_081658C8(p);
    PauseMenu_UpdateHighlightPalette(p);
    if (p->statusRefreshRequested)
        goto close;
    if (p->comparisonModifier) {
        PauseMenu_LoadSpriteKeyframe(p, 1, 0x43);
        PauseMenu_LoadSpriteKeyframe(p, 2, 0x44);
        sub_0824923C(p, p->pageCallback);
        return;
    }
    PauseMenu_LoadSpriteKeyframe(p, 1, 0x41);
    PauseMenu_LoadSpriteKeyframe(p, 2, 0x42);
    if (gUnk_03005260[1] & 2) {
        Sound_PlaySong(0x119);
    close:
        Hud_SetUpdateCallback(p, sub_08176B00);
        return;
    }
    if (!p->pageInputCooldown) {
        direction = 0;
        if (gUnk_03005260[1] & 0x100) {
            p->pageRepeatTimer = 0;
            direction = 1;
        } else if (gUnk_03005260[1] & 0x200) {
            p->pageRepeatTimer = 0;
            direction = 2;
        } else if (gUnk_03005260[0] & 0x100) {
            if (++p->pageRepeatTimer > 9) {
                p->pageRepeatTimer = 0;
                direction = 1;
            }
        } else if (gUnk_03005260[0] & 0x200) {
            if (++p->pageRepeatTimer > 9) {
                p->pageRepeatTimer = 0;
                direction = 2;
            }
        }
        if (direction == 1) {
            Sound_PlaySong(0x10A);
            p->mode = sub_08176838(p, p->mode);
        } else if (direction == 2) {
            Sound_PlaySong(0x10A);
            p->mode = sub_08176874(p, p->mode);
        } else
            goto callback;
        sub_08176590(p);
        return;
    } else
        --p->pageInputCooldown;
callback:
    sub_0824923C(p, p->pageCallback);
}
