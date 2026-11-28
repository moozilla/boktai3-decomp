#ifndef GUARD_TITLE_DISPLAY_H
#define GUARD_TITLE_DISPLAY_H

#include "ui/title/text_entry_widget.h"

/* confidence=low: menu ownership follows TitleDisplay_Create; option meanings remain tentative. */
struct Display {
    u8 task[0x18];
    struct TextEntryWidget widgets[9];
    u8 keyframes[0x20]; /* 0x4E0: the widgets' keyframe resource (size a guess, confidence=low) */
    u16 palettes
        [17][16]; /* 0x500: palette banks; 0 is widget 1's, 16 the blend output (confidence=low) */
    void *resource; /* 0x720: background graphics */
    s32 selection, count;
    u32 state;
    s32 result, counter, fadeShift, idleTimer;
    s32 pairFrame, pairTicks; /* 0x740: the paired sprites' animation */
    s32 stateTimer;
    u32 lastState;
    s32 velocity, timer, slide, paletteStep, unused760, skipped, musicTimer;
};

#endif
