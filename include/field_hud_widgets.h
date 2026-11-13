#ifndef GUARD_FIELD_HUD_WIDGETS_H
#define GUARD_FIELD_HUD_WIDGETS_H
#include "global.h"
#include "scene_input_widget.h"
/* Field HUD block at SceneHudState + 0x492C. */
struct FieldHudWidgets {
    s32 *source;
    s32 value, displayedValue;
    u16 numberResource[16]; /* 0x0C: palette for current/initial value display */
    s16 numberStep;         /* 0x2C: low-value warning threshold */
    s16 deltaR, deltaG, deltaB, timer;
    u16 colorStart, numberColor;
    s16 colorPhase;
    s32 red, green, blue;
    u8 unknown48[4];
    const u16 *paletteResource; /* 0x4C */
    s32 fill, displayedIndex;
    s32 *gaugeSource;
    struct SceneInputWidget meterWidgets[2]; /* 0x5C: participants' viewport positions */
    u8 meterResource[0x20];                  /* 0x11C */
    void *meterScript;
    s32 meterScale;
    struct SceneInputWidget timeDigits[6]; /* 0x144 */
    u8 timeResource[0x20];                 /* 0x384 */
    void *timeScript;
    s32 *timeSource; /* 0x3A8: elapsed frames */
};
#endif
