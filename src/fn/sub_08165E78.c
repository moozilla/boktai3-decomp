#include "global.h"
#include "scene_hud_display.h"

#define SceneHud_BeginWidgetMotion SceneHud_BeginIconMotion
#define SceneHud_SetWidgetPalette  PauseMenu_SetSpritePalette
#define gSineTable                 gUnk_086149C4
s32 Math_Atan2(s32, s32);
void PauseMenu_SetSpritePalette(struct SceneInputWidget *, u32);
static inline s32 ScaleBend(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline s32 BendCosine(s32 angle, s32 bend)
{
    return ScaleBend(gSineTable[(angle + 0x40) & 255] * bend);
}
static inline s32 BendSine(s32 angle, s32 bend)
{
    const s16 *tbl = gSineTable;
    s32 wrapped = 255;
    wrapped &= angle;
    return ScaleBend(tbl[wrapped] * bend);
}
void SceneHud_BeginIconMotion(struct SceneHudState *hud, u32 index, u32 keyframe, u32 palette,
                              struct SceneVec *from, struct SceneVec *to, s32 bend, u32 duration)
{
    struct SceneHudWidgetMotion *motion = &hud->widgetMotions[index];
    struct SceneInputWidget *widget;
    motion->from = *from;
    motion->to = *to;
    if (bend != 0) {
        s32 angle = (Math_Atan2(to->x - from->x, to->y - from->y) + 0x40) & 255;
        motion->bendX = BendCosine(angle, bend);
        asm("" : "+r"(angle));
        motion->bendY = BendSine(angle, bend);
    } else {
        motion->bendX = bend;
        motion->bendY = bend;
    }
    widget = &hud->motionWidgets[index];
    Anim_LoadKeyframe(widget, hud->motionResource, (u16)keyframe, 1);
    PauseMenu_SetSpritePalette(widget, palette);
    *(struct SceneVec *)&widget->position = motion->from;
    widget->flags &= ~1;
    motion->timer = 0;
    motion->duration = duration;
    motion->active = 1;
}
