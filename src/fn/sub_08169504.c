#include "global.h"
#include "scene_hud_display.h"

static inline void PauseMenu_SetWidgetPosition(struct SceneVec *v, s16 x, s16 y, s16 z, s16 pad)
{
    s32 padding = pad;
    s16 *xPosition = &v->x;
    *xPosition = x;
    v->y = y;
    v->z = z;
    v->pad = padding;
}

void PauseMenu_CreateSelectionWidgets(struct SceneHudState *p)
{
    s16 primaryPos[4];
    s16 secondaryPos[4];
    s16 tertiaryPos[4];
    struct SceneVec pos;
    struct SceneVec *position;
    s32 i;

    PauseMenu_SetWidgetPosition((struct SceneVec *)primaryPos, 8, 0x88, 0, 0);
    secondaryPos[0] = 0xD8;
    secondaryPos[1] = 0x88;
    secondaryPos[2] = 0;
    secondaryPos[3] = 0;
    tertiaryPos[0] = 0xB8;
    tertiaryPos[1] = 0x88;
    tertiaryPos[2] = 0;
    tertiaryPos[3] = 0;
    for (i = 0; i <= 2; i++) {
        Widget_InitLinked(&p->primaryWidgets[i], p->motionResource, 0x184, 0x10011, 0, 0, 60,
                          primaryPos);
        Widget_InitLinked(&p->secondaryWidgets[i], p->motionResource, 0xF0, 0x10011, 0, 0, 60,
                          secondaryPos);
        tertiaryPos[1] = 0x88 - (i * 0x18);
        Widget_InitLinked(&p->tertiaryWidgets[i], p->motionResource, 0xE7, 0x10011, 0, 0, 60,
                          tertiaryPos);
    }

    position = &pos;
    pos.x = 0;
    position->y = 0;
    position->z = 0;
    position->pad = 0;
    Widget_InitLinked(&p->extraWidgetsA[0], p->widgetResource, 0x90, 0x10011, 0, 0, 60,
                      (s16 *)position);
    pos.x = 0xB0;
    position->y = 0;
    position->z = 0;
    position->pad = 0;
    Widget_InitLinked(&p->extraWidgetsA[1], p->widgetResource, 0x90, 0x10011, 0, 0, 60,
                      (s16 *)position);
    pos.x = 0xD0;
    position->y = 0;
    position->z = 0;
    position->pad = 0;
    Widget_InitLinked(&p->extraWidgetsA[2], p->widgetResource, 0x90, 0x10011, 0, 0, 60,
                      (s16 *)position);
    pos.x = 0xC;
    position->y = 0;
    position->z = 0;
    position->pad = 0;
    Widget_InitLinked(&p->extraWidgetsB[0], p->widgetResource, 0x90, 0x10011, 0, 0, 60,
                      (s16 *)position);
    pos.x = 0xBC;
    position->y = 0;
    position->z = 0;
    position->pad = 0;
    Widget_InitLinked(&p->extraWidgetsB[1], p->widgetResource, 0x90, 0x10011, 0, 0, 60,
                      (s16 *)position);
    pos.x = 0xDC;
    position->y = 0;
    position->z = 0;
    position->pad = 0;
    Widget_InitLinked(&p->extraWidgetsB[2], p->widgetResource, 0x90, 0x10011, 0, 0, 60,
                      (s16 *)position);
    pos.x = 0;
    position->y = 0;
    position->z = 0;
    position->pad = 0;
    Widget_InitLinked(&p->extraWidgetsC[0], p->widgetResource, 0x8F, 0x10011, 0, 0, 60,
                      (s16 *)position);
    pos.x = 0xB0;
    position->y = 0;
    position->z = 0;
    position->pad = 0;
    Widget_InitLinked(&p->extraWidgetsC[1], p->widgetResource, 0x8F, 0x10011, 0, 0, 60,
                      (s16 *)position);
    pos.x = 0xD0;
    position->y = 0;
    position->z = 0;
    position->pad = 0;
    Widget_InitLinked(&p->extraWidgetsC[2], p->widgetResource, 0x8F, 0x10011, 0, 0, 60,
                      (s16 *)position);
}
