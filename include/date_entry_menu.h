#ifndef GUARD_DATE_ENTRY_MENU_H
#define GUARD_DATE_ENTRY_MENU_H

#include "scene_input_widget.h"
#include "text_entry_widget.h"

struct DateMenu {
    u8 task[0x18];
    struct TextEntryWidget widgets[4];
    struct SceneInputWidget digits[12];
    struct SceneInputResource digitResource;
    s32 selection, timer, fadeShift, state, result;
    s32 year, month, day, hour, minute;
    s32 oldYear, oldMonth, oldDay, oldHour, oldMinute;
    s32 repeatTimer;
    u16 *savedPalette;
    u16 colors[16];
    u8 pulseState[4];
};

s32 Time_InitializeDateMenu(struct DateMenu *);
void sub_081DE630(struct DateMenu *);

typedef char DateMenuSizeCheck[sizeof(struct DateMenu) == 0x740 ? 1 : -1];

#endif
