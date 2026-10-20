#include "global.h"
#include "scene_hud_display.h"

#ifdef NONMATCHING
// fuzzy: 81.9%
// Remaining difference: the ROM stores each position's x through a separate
// pointer register (sp+0x28 in r6 after the loop, r7 for y/z/w and the call
// argument, set up before the loop); arrays, structs, pointer locals and an
// inline setter all failed to reproduce that split.

void PauseMenu_CreateSelectionWidgets(struct SceneHudState *p)
{
    s16 primaryPos[4], secondaryPos[4], tertiaryPos[4], pos[4];
    s32 i;

    primaryPos[0] = 8;
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
        sub_0821983C(&p->primaryWidgets[i], p->motionResource, 0x184, 0x10011, 0, 0, 60,
                     primaryPos);
        sub_0821983C(&p->secondaryWidgets[i], p->motionResource, 0xF0, 0x10011, 0, 0, 60,
                     secondaryPos);
        tertiaryPos[1] = 0x88 - i * 0x18;
        sub_0821983C(&p->tertiaryWidgets[i], p->motionResource, 0xE7, 0x10011, 0, 0, 60,
                     tertiaryPos);
    }
#define EXTRA(widget, frame, X)                                                                    \
    pos[0] = X;                                                                                    \
    pos[1] = 0;                                                                                    \
    pos[2] = 0;                                                                                    \
    pos[3] = 0;                                                                                    \
    sub_0821983C(widget, p->widgetResource, frame, 0x10011, 0, 0, 60, pos);
    EXTRA(&p->extraWidgetsA[0], 0x90, 0)
    EXTRA(&p->extraWidgetsA[1], 0x90, 0xB0)
    EXTRA(&p->extraWidgetsA[2], 0x90, 0xD0)
    EXTRA(&p->extraWidgetsB[0], 0x90, 0xC)
    EXTRA(&p->extraWidgetsB[1], 0x90, 0xBC)
    EXTRA(&p->extraWidgetsB[2], 0x90, 0xDC)
    EXTRA(&p->extraWidgetsC[0], 0x8F, 0)
    EXTRA(&p->extraWidgetsC[1], 0x8F, 0xB0)
    EXTRA(&p->extraWidgetsC[2], 0x8F, 0xD0)
}
#else
INCLUDE_ASM("asm/nonmatching", PauseMenu_CreateSelectionWidgets);
#endif
