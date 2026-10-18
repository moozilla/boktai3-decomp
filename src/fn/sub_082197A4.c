#define gAnimationWidgetStamp gUnk_03005238
#include "global.h"
#include "scene_input.h"

extern u16 gUnk_03005238;

s32 Anim_InitPositionedWidget(struct SceneInputWidget *widget, void *resource, u16 glyph, u32 flags,
                              u8 attributeA, u8 attributeB, u8 attributeC,
                              const struct ContactPosition *position)
{
    if (Anim_SelectWidgetGlyph(widget, resource, glyph) < 0)
        return -1;
    widget->stamp = gUnk_03005238;
    widget->flags |= flags;
    widget->fieldC = 0;
    widget->scale = 0x40;
    widget->attributeE = attributeC;
    widget->attribute1A = attributeA;
    widget->attribute1B = attributeB;
    widget->position = *position;
    return 0;
}
