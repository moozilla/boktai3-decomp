#include "scene_inventory_tail.h"
#include "scene_inventory_panel.h"
struct SceneInventoryInitTransitions {
    u8 unknown0[0x1AF0];
    struct SceneInputPair transition;
    struct SceneInputGridView grid;
    struct SceneInputPair prompt;
};
void SceneInputPanel_InitHighlight(struct SceneInputPanel *, struct SceneInputHighlight *);
void SceneInventory_InitPanel(struct SceneInventoryTail *p, s32 mode)
{
    struct SceneInputPackedPosition position;
    struct SceneInputResource *resource;
    s32 i;
    position.z = position.y = position.x = 0;
    SceneInputPanel_InitWidgets(&p->display, mode);
    SceneInputPanel_InitHighlight(&p->display, &p->highlight);
    position.x = 64;
    position.y = 88;
    {
        struct SceneInputPackedPosition *location = &position;
        struct SceneInputWidget *widget;
        resource = &p->display.widgetResource;
        widget = ((struct SceneInventoryPanel *)p)->comparison[0];
        for (i = 0; i < 3; i++) {
            Widget_InitLinked(widget, &p->display.widgetResource, 0, 0x30, 0, 0, 60, location);
            location->x += 8;
            widget++;
        }
    }
    position.x = 64;
    position.y = 80;
    {
        struct SceneInputPackedPosition *location = &position;
        struct SceneInputWidget *widget = ((struct SceneInventoryPanel *)p)->comparison[1];
        for (i = 0; i < 3; i++) {
            Widget_InitLinked(widget, resource, 0, 0x30, 0, 0, 60, location);
            location->x += 8;
            widget++;
        }
    }
    position.x = 80;
    position.y = 96;
    {
        struct SceneInputPackedPosition *location = &position;
        struct SceneInputWidget *widget = ((struct SceneInventoryPanel *)p)->comparison[2];
        for (i = 0; i < 3; i++) {
            Widget_InitLinked(widget, resource, 0, 0x30, 0, 0, 60, location);
            location->x += 8;
            widget++;
        }
    }
    position.x = 80;
    position.y = 104;
    {
        struct SceneInputPackedPosition *location = &position;
        struct SceneInputWidget *widget = ((struct SceneInventoryPanel *)p)->comparison[3];
        for (i = 0; i < 3; i++) {
            Widget_InitLinked(widget, resource, 0, 0x30, 0, 0, 60, location);
            location->x += 8;
            widget++;
        }
    }
    position.x = 80;
    position.y = 112;
    {
        struct SceneInputPackedPosition *location = &position;
        struct SceneInputWidget *widget = ((struct SceneInventoryPanel *)p)->comparison[4];
        for (i = 0; i < 3; i++) {
            Widget_InitLinked(widget, resource, 0, 0x30, 0, 0, 60, location);
            location->x += 8;
            widget++;
        }
    }
    SceneInput_InitMarkerPair(&((struct SceneInventoryInitTransitions *)p)->transition);
    SceneInput_InitGlyphPair(&((struct SceneInventoryInitTransitions *)p)->prompt);
}
