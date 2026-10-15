// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_tail.h"
#include "scene_inventory_panel.h"
struct SceneInventoryInitTransitions {
    u8 unknown0[0x1AF0];
    struct SceneInputPair transition;
    struct SceneInputGridView grid;
    struct SceneInputPair prompt;
};
void SceneInputPanel_InitHighlight(struct SceneInputPanel *, struct SceneInputHighlight *);
void SceneInventory_InitPanel(struct SceneInventoryTail *input, s32 mode)
{
    struct SceneInventoryTail *p = input;
    struct SceneInputPackedPosition position;
    register struct SceneInputPackedPosition *location asm("r6");
    struct SceneInputWidget *widget;
    struct SceneInputResource *resource;
    register s32 i asm("r4");
    u32 zero;
    position.z = position.y = position.x = 0;
    SceneInputPanel_InitWidgets(&p->display, mode);
    SceneInputPanel_InitHighlight(&p->display, &p->highlight);
    position.x = 64;
    position.y = 88;
    location = &position;
    resource = &p->display.widgetResource;
    widget = ((struct SceneInventoryPanel *)p)->comparison[0];
    zero = 0;
    i = 2;
    do {
        sub_0821983C(widget, &p->display.widgetResource, 0, 0x30, zero, zero, 60, location);
        location->x += 8;
        widget++;
    } while (--i >= 0);
    position.x = 64;
    position.y = 80;
    location = &position;
    widget = ((struct SceneInventoryPanel *)p)->comparison[1];
    zero = 0;
    asm("" : "+l"(zero));
    i = 2;
    do {
        sub_0821983C(widget, resource, 0, 0x30, zero, zero, 60, location);
        location->x += 8;
        widget++;
    } while (--i >= 0);
    position.x = 80;
    position.y = 96;
    location = &position;
    widget = ((struct SceneInventoryPanel *)p)->comparison[2];
    zero = 0;
    i = 2;
    do {
        sub_0821983C(widget, resource, 0, 0x30, zero, zero, 60, location);
        location->x += 8;
        widget++;
    } while (--i >= 0);
    position.x = 80;
    position.y = 104;
    location = &position;
    widget = ((struct SceneInventoryPanel *)p)->comparison[3];
    zero = 0;
    i = 2;
    do {
        sub_0821983C(widget, resource, 0, 0x30, zero, zero, 60, location);
        location->x += 8;
        widget++;
    } while (--i >= 0);
    position.x = 80;
    position.y = 112;
    location = &position;
    widget = ((struct SceneInventoryPanel *)p)->comparison[4];
    zero = 0;
    i = 2;
    do {
        sub_0821983C(widget, resource, 0, 0x30, zero, zero, 60, location);
        location->x += 8;
        widget++;
    } while (--i >= 0);
    SceneInput_InitMarkerPair(&((struct SceneInventoryInitTransitions *)p)->transition);
    SceneInput_InitGlyphPair(&((struct SceneInventoryInitTransitions *)p)->prompt);
}
