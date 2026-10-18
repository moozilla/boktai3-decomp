#define SceneHud_LayoutActionGrid sub_08172950
#define SceneHud_SetWidgetPalette PauseMenu_SetSpritePalette
#include "global.h"
#include "scene_hud_display.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
void PauseMenu_SetSpritePalette(struct SceneInputWidget *, u16);
void sub_08172638(void *);
static inline void SetPosition(struct SceneVec *position, s16 x, s16 y, s16 z)
{
    position->x = x;
    position->y = y;
    position->z = z;
    position->pad = 0;
}
void sub_08172950(struct SceneHudState *p)
{
    struct SceneVec position;
    s32 i;
    struct SceneInputWidget *w;
    SetPosition(&position, 0x50, 0x20, 0);
    for (i = 0; i <= 7; i++) {
        p->categoryWidgets[i + 10].position = *(struct ContactPosition *)&position;
        p->categoryWidgets[i + 10].position.x += (i & 1) * 24;
        p->categoryWidgets[i + 10].position.y += (i >> 1) * 24;
    }
    SetPosition(&position, 0x88, 0x20, 0);
    for (i = 0; i <= 11; i++) {
        p->categoryWidgets[i + 18].position = *(struct ContactPosition *)&position;
        p->categoryWidgets[i + 18].position.x += Mod(i, 3) * 24;
        p->categoryWidgets[i + 18].position.y += Div(i, 3) * 24;
    }
    sub_08172638(p);
    {
        struct SceneInputWidget *a = &p->actionPanelWidgets[0];
        a->flags &= ~1;
    }
    {
        struct SceneInputWidget *a = &p->actionPanelWidgets[1];
        a->flags &= ~1;
    }
    w = &p->actionPanelWidgets[2];
    w->flags &= ~1;
    PauseMenu_SetSpritePalette(w, 0x373);
}
