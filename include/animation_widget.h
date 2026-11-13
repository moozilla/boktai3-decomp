#ifndef GUARD_ANIMATION_WIDGET_H
#define GUARD_ANIMATION_WIDGET_H
#include "scene_input_widget.h"
#include "render.h"
#include "gba/gba.h"
struct WidgetGlyphEntry {
    u16 kind, resourceIndex, width, height, originX, originY, centerX, centerY;
    u32 tileOffset;
};
struct WidgetGlyphResource {
    u16 paletteIndex, glyphCount;
    u32 unknown4, unknown8;
    struct WidgetGlyphEntry *glyphs;
    u32 unknown10;
    u32 tileBase;
    u32 unknown18;
    u32 extraData;
};
s32 Anim_SelectWidgetGlyph(struct SceneInputWidget *, struct WidgetGlyphResource *, u16);
s32 Anim_InitPositionedWidget(struct SceneInputWidget *, void *, u16, u32, u8, u8, u8,
                              const struct ContactPosition *);
void Anim_Remove(struct SceneInputWidget *);
#endif
