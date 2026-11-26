#ifndef GUARD_ANIMATION_WIDGET_H
#define GUARD_ANIMATION_WIDGET_H
#include "ui/widgets/scene_input_widget.h"
#include "engine/video/render.h"
#include "gba/gba.h"
struct WidgetGlyphEntry {
    u16 kind, resourceIndex, width, height, originX, originY, centerX, centerY;
    u32 tileOffset;
};
s32 Anim_SelectWidgetGlyph(struct SceneInputWidget *, struct SceneInputResource *, u16);
s32 Anim_InitPositionedWidget(struct SceneInputWidget *, void *, u16, u32, u8, u8, u8,
                              const struct ContactPosition *);
void Anim_Remove(struct SceneInputWidget *);
#endif
