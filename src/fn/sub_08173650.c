#define SceneInput_SetWidgetPalette PauseMenu_SetSpritePalette
#define SceneHud_LayoutEquipmentGrid sub_08173650
#include "global.h"
#include "scene_hud_display.h"
void sub_0817373C(void *, int);
void PauseMenu_SetSpritePalette(void *, u16);
static inline void InitPosition(struct ContactPosition *v, s16 x, s16 y, s16 z, s16 pad)
{
    v->x = x;
    v->y = y;
    v->z = z;
    v->pad = pad;
}
void sub_08173650(struct SceneHudState *p)
{
    struct ContactPosition position[1];
    s32 i;
    struct ContactPosition *v = position;
    InitPosition(v, 0x70, 0x20, 0, 0);
    i = 0;
    {
        u32 base = 0x1440;
        s16 *x = &p->categoryWidgets[10].position.x;
        u8 *ybase = (u8 *)p + 0x3c0;
        u32 offset = 0x3c0;
        for (; i <= 15; i++) {
            *(struct ContactPosition *)((u8 *)p + base + offset) = position[0];
            *x += (i & 3) * 24;
            *(s16 *)(ybase + 0x1442) += (i >> 2) * 24;
            x = (void *)((u8 *)x + 0x60);
            ybase += 0x60;
            offset += 0x60;
        }
    }
    sub_0817373C(p, 0);
    {
        struct SceneInputWidget *a = (void *)((u8 *)p + 0x2560);
        a->flags &= ~1;
        {
            struct SceneInputWidget *b = (void *)((u8 *)p + 0x25c0);
            b->flags &= ~1;
        }
        {
            struct SceneInputWidget *c = (void *)((u8 *)p + 0x2680);
            c->flags &= ~1;
            PauseMenu_SetSpritePalette(c, 0x373);
        }
        {
            struct SceneInputWidget *d = (void *)((u8 *)p + 0x2620);
            d->flags &= ~1;
            if (p->actionInput[0] & 0x40)
                PauseMenu_SetSpritePalette(d, 0x373);
            else
                PauseMenu_SetSpritePalette(d, 0x374);
        }
    }
}
