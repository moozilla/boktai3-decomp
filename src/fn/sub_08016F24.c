#include "global.h"
#include "engine.h"
#include "choice_menu.h"
struct Position {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct LinkedWidgetSlot {
    u32 value;
    u8 visible, pad5, state, pad7;
    u8 pad8[4];
    u32 timer;
    u8 pad10[0x24 - 0x10];
    u8 widget[0x84 - 0x24];
};
struct LinkedWidgetSet {
    u8 task[0x18];
    u32 unknown18;
    struct ChoiceResource resource;
    struct LinkedWidgetSlot slots[8];
};
extern struct LinkedWidgetSet *gUnk_02000078;
void Widget_InitLinked(void *, struct ChoiceResource *, s32, s32, s32, s32, s32, struct Position *);
// confidence=low: loads resource CB05/5D04 and sets up eight linked widgets with frame 0x11.
s32 LinkedWidgetSet_Init(struct LinkedWidgetSet *p)
{
    struct ChoiceResource *resource;
    struct Position zero;
    s32 i;
    gUnk_02000078 = p;
    resource = ScriptProc_Start(0xCB05, 0x5D04);
    if (!resource)
        return -1;
    p->resource = *resource;
    ResourceHeader_Relocate(&p->resource, resource);
    p->unknown18 = 0;
    ZeroFill8(&zero);
    for (i = 0; i < 8; i++) {
        struct LinkedWidgetSlot *slot = &p->slots[i];
        slot->value = 0;
        slot->visible = 1;
        slot->state = 0;
        slot->timer = 0;
        Widget_InitLinked(slot->widget, &p->resource, 0, 0x11, 1, 0, 0x3C, &zero);
    }
}
