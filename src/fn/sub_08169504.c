#include "global.h"
#include "scene_hud_display.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining difference: the ROM stores each position's x through a separate
// pointer register (sp+0x28 in r6 after the loop, r7 for y/z/w and the call
// argument, set up before the loop); arrays, structs, pointer locals and an
// inline setter all failed to reproduce that split.

void PauseMenu_CreateSelectionWidgets(struct SceneHudState *p)
{
    s16 primaryPos[4];
    s16 secondaryPos[4];
    s16 tertiaryPos[4];
    s16 pos[4];
    s32 i;
    do {
        primaryPos[0] = 8;
    } while (0);
    primaryPos[1] = 0x88;
    primaryPos[2] = 0;
    primaryPos[3] = 0;
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

    pos[0] = 0;
    pos[1] = 0;
    pos[2] = 0;
    pos[3] = 0;
    Widget_InitLinked(&p->extraWidgetsA[0], p->widgetResource, 0x90, 0x10011, 0, 0, 60, pos);
    pos[0] = 0xB0;
    pos[1] = 0;
    pos[2] = 0;
    pos[3] = 0;
    Widget_InitLinked(&p->extraWidgetsA[1], p->widgetResource, 0x90, 0x10011, 0, 0, 60, pos);
    pos[0] = 0xD0;
    pos[1] = 0;
    pos[2] = 0;
    pos[3] = 0;
    Widget_InitLinked(&p->extraWidgetsA[2], p->widgetResource, 0x90, 0x10011, 0, 0, 60, pos);
    pos[0] = 0xC;
    pos[1] = 0;
    pos[2] = 0;
    pos[3] = 0;
    Widget_InitLinked(&p->extraWidgetsB[0], p->widgetResource, 0x90, 0x10011, 0, 0, 60, pos);
    pos[0] = 0xBC;
    pos[1] = 0;
    pos[2] = 0;
    pos[3] = 0;
    Widget_InitLinked(&p->extraWidgetsB[1], p->widgetResource, 0x90, 0x10011, 0, 0, 60, pos);
    pos[0] = 0xDC;
    pos[1] = 0;
    pos[2] = 0;
    pos[3] = 0;
    Widget_InitLinked(&p->extraWidgetsB[2], p->widgetResource, 0x90, 0x10011, 0, 0, 60, pos);
    pos[0] = 0;
    pos[1] = 0;
    pos[2] = 0;
    pos[3] = 0;
    Widget_InitLinked(&p->extraWidgetsC[0], p->widgetResource, 0x8F, 0x10011, 0, 0, 60, pos);
    pos[0] = 0xB0;
    pos[1] = 0;
    pos[2] = 0;
    do {
        pos[3] = 0;
    } while (0);
    Widget_InitLinked(&p->extraWidgetsC[1], p->widgetResource, 0x8F, 0x10011, 0, 0, 60, pos);
    pos[0] = 0xD0;
    pos[1] = 0;
    pos[2] = 0;
    pos[3] = 0;
    Widget_InitLinked(&p->extraWidgetsC[2], p->widgetResource, 0x8F, 0x10011, 0, 0, 60, pos);
}

#else
INCLUDE_ASM("asm/nonmatching", PauseMenu_CreateSelectionWidgets);
#endif
