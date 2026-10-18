#define SceneHud_LayoutAlternateActionGrid sub_08170C90
#define SceneHud_SetWidgetPalette PauseMenu_SetSpritePalette
#include "global.h"
#include "scene_hud_display.h"
void SceneHud_SetWidgetPalette(struct SceneInputWidget *, u16);
void sub_08170D78(void *);
void sub_08170C90(struct SceneHudState *s)
{
    struct SceneVec pos;
    s32 i;

    {
        struct ContactPosition *v = (void *)&pos;
        u16 x = 112, y = 32, zero = 0;
        v->x = x;
        v->y = y;
        v->z = zero;
        v->pad = zero;
    }
    for (i = 0; i < 16; i++) {
        s->categoryWidgets[i + 10].position = *(struct ContactPosition *)&pos;
        s->categoryWidgets[i + 10].position.x += (i & 3) * 24;
        s->categoryWidgets[i + 10].position.y += (i >> 2) * 24;
    }
    sub_08170D78(s);
    ((struct SceneInputWidget *)((u8 *)s + 0x2560))->flags &= ~1;
    ((struct SceneInputWidget *)((u8 *)s + 0x25c0))->flags &= ~1;
    {
        struct SceneInputWidget *w;
        w = (void *)((u8 *)s + 0x2680);
        w->flags &= ~1;
        SceneHud_SetWidgetPalette(w, 0x373);
    }
    {
        struct SceneInputWidget *w = (void *)((u8 *)s + 0x2620);
        w->flags &= ~1;
        if (s->actionInput[0] & 0x40)
            SceneHud_SetWidgetPalette(w, 0x373);
        else
            SceneHud_SetWidgetPalette(w, 0x374);
    }
}
