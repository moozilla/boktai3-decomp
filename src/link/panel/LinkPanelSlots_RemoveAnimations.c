#include "global.h"
#include "link/panel/link_panel.h"

void LinkPanelSlots_RemoveAnimations(struct LinkPanel *panel)
{
    s32 i;

    for (i = 0; i < 7; i++)
        Anim_Remove(&panel->animations[i]);
}
