#define gAnimationWidgetStamp gDefaultAnimFlags
#include "global.h"
#include "scene_input.h"
#include "render.h"

s32 Anim_InitPositionedWidget(struct SceneInputWidget *widget, void *resource, u16 glyph, u32 flags,
                              u8 attributeA, u8 attributeB, u8 attributeC,
                              const struct ContactPosition *position)
{
    if (Anim_SelectWidgetGlyph(widget, resource, glyph) < 0)
        return -1;
    widget->stamp = gDefaultAnimFlags;
    widget->flags |= flags;
    widget->fieldC = 0;
    widget->scale = 0x40;
    widget->attributeE = attributeC;
    widget->attribute1A = attributeA;
    widget->attribute1B = attributeB;
    widget->position = *position;
    return 0;
}
