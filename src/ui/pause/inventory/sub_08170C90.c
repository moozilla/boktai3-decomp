#define SceneHud_SetWidgetPalette PauseMenu_SetSpritePalette
#include "global.h"
#include "ui/pause/scene_hud_display.h"
void SceneHud_SetWidgetPalette(struct AnimSprite *, u16);
void sub_08170D78(void *);
void SceneHud_LayoutAlternateActionGrid(struct SceneHudState *s)
{
    struct SceneVec pos;
    s32 i;

    {
        struct ContactPosition *v = (void *)&pos;
        u16 x = 112, y = 32;
        v->x = x;
        v->y = y;
        v->z = 0;
        v->pad = 0;
    }
    for (i = 0; i < 16; i++) {
        s->categoryWidgets[i + 10].position = *(struct ContactPosition *)&pos;
        s->categoryWidgets[i + 10].position.x += (i & 3) * 24;
        s->categoryWidgets[i + 10].position.y += (i >> 2) * 24;
    }
    sub_08170D78(s);
    ((struct AnimSprite *)((u8 *)s + 0x2560))->flags &= ~1;
    ((struct AnimSprite *)((u8 *)s + 0x25c0))->flags &= ~1;
    {
        struct AnimSprite *w;
        w = (void *)((u8 *)s + 0x2680);
        w->flags &= ~1;
        SceneHud_SetWidgetPalette(w, 0x373);
    }
    {
        struct AnimSprite *w = &s->actionPanelWidgets[2];
        w->flags &= ~1;
        if (s->actionInput[0] & 0x40)
            SceneHud_SetWidgetPalette(w, 0x373);
        else
            SceneHud_SetWidgetPalette(w, 0x374);
    }
}
