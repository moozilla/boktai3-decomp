#include "global.h"
#include "ram.h"
struct SelectionPanelAnimation {
    u32 unknown0;
    u8 active;
    u8 unknown5[0x5B];
};
struct SelectionPanel {
    u8 unknown0[0x20];
    struct SelectionPanelAnimation animations[13];
    u8 unknown500[0x58];
    void *list;
};
void Anim_Remove(void *);
void SelectionPanel_Destroy(struct SelectionPanel *panel)
{
    struct SelectionPanelAnimation *animation = panel->animations;
    s32 i = 12;
    do {
        if (animation->active)
            Anim_Remove(animation);
        ++animation;
    } while (--i >= 0);
    if (panel->list)
        panel->list = 0;
    gSelectionPanel = 0;
}
