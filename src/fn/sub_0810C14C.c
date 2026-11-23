#include "global.h"
#include "link_panel.h"

void RegistryF246_HideAnimations(struct LinkPanel *panel)
{
    s32 i;

    for (i = 0; i < 7; i++)
        panel->animations[i].flags |= 1;
}
