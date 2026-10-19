#define SceneHud_TickWidgetSlides sub_08168EE8
#define SceneHud_GetSlidingWidgetY sub_081686E8
#include "global.h"
#include "scene_hud_display.h"
s32 sub_081686E8(struct SceneHudState *, s32, s32, s32, s32);
void sub_08168EE8(struct SceneHudState *p)
{
    s16 *q;
    s32 i;
    if (p->primarySlideTimer) {
        p->primarySlideTimer--;
        i = 0;
        q = &p->primaryWidgets[0].position.y;
        for (; i <= 2; i++) {
            *q = sub_081686E8(p, i, p->primarySelection, 3, p->primarySlideTimer);
            q += 0x30;
        }
    }
    if (p->secondarySlideTimer) {
        p->secondarySlideTimer--;
        i = 0;
        q = &p->secondaryWidgets[0].position.y;
        for (; i <= 2; i++) {
            *q = sub_081686E8(p, i, p->selection, 3, p->secondarySlideTimer);
            q += 0x30;
        }
    }
    if (p->tertiarySlideTimer) {
        p->tertiarySlideTimer--;
        i = 0;
        q = &p->tertiaryWidgets[0].position.y;
        for (; i <= 2; i++) {
            *q = sub_081686E8(p, i, p->tertiarySelection, 3, p->tertiarySlideTimer);
            q += 0x30;
        }
    }
}
