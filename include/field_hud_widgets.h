#ifndef FOUNDRY_FIELD_HUD_WIDGETS_H
#define FOUNDRY_FIELD_HUD_WIDGETS_H
#include "global.h"
/* Tentative field HUD view. Only offsets observed in this cohort are exposed. */
struct FieldHudWidgets {
    s32 *source; /* 0x00 */
    s32 value;
    s32 displayedValue; /* clamped to 999 */
    u16 numberResource[8];
    u8 unknown1C[0x10];
    u16 numberStep; /* 0x2C: initial value * 3 / 10 */
    u8 unknown2E[0xA];
    u16 numberColor; /* 0x38 */
    u8 unknown3A[0x12];
    void *paletteResource; /* 0x4C */
    s32 fill;              /* 0x50 */
    s32 displayedIndex;
    s32 **gaugeSource; /* 0x58 */
};
struct FieldHud {
    u8 unknown0[0xA5E];
    u8 mode;
    u8 unknownA5F[7];
    u16 valueMode;    /* 0xA66 */
    u16 *valueSource; /* 0xA68: low halfword consumed by refresh */
    u8 unknownA6C[0x492C - 0xA6C];
    struct FieldHudWidgets widgets;
    u8 unknown4988[0x4CD8 - 0x4988];
    u8 widgetsVisible;
};
#endif
