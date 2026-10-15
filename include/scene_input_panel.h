#ifndef GUARD_SCENE_INPUT_PANEL_H
#define GUARD_SCENE_INPUT_PANEL_H
/* Tentative display owner for the nine-column scene-input grid. */
#include "scene_input.h"
struct SceneInputPanelDrawable {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
    u8 graphics[0x1C];
};
struct SceneInputPanel {
    u8 unknown0[0x18];
    void *backgroundResource;
    const u8 *labelResource;
    struct SceneInputResource widgetResource, gridResource;
    struct SceneInputPanelDrawable drawable;
    struct SceneInputWidget indicator, valueA, valueB, prompt, mode;
    struct SceneInputWidget primary[16], secondary[8], buttons[4];
    struct SceneInputWidget banner, compareA, category, modeCursor;
    struct SceneInputWidget heading, valueLabel, glyph, meterGlyph;
    struct SceneInputWidget digits[8], numericLabel, smallDigits[4];
};
struct SceneInputHighlight {
    u16 colors[16];
    u8 phase;
};
void sub_0805F324(struct SceneInputPanel *, struct SceneInputHighlight *);
extern u8 gUnk_03005160[];
extern const u16 gUnk_0824DEEA[], gUnk_0824DEF4[], gUnk_0824DF14[], gUnk_0824DF1E[],
    gUnk_0824DEE0[];
extern const u32 gUnk_0824DF00[];
void sub_08220D20(struct DisplaySequenceState *, void *, u32, u32, u32);
void sub_0805EB0C(struct SceneInputPanel *, s32);
void sub_0805EA58(struct SceneInputPanel *, s32, u32);
#endif
