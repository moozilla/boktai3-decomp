#include "global.h"
#include "link/panel/link_panel.h"
#include "scene/areas/area_effect_link.h"

void LinkPanel_SelectActiveAnimation(struct LinkPanel *panel, s32 i)
{
    struct AreaEffectLinkState *session = gLinkSession;
    s32 mode = -1;

    if (session)
        mode = session->unknown1C;
    if (mode <= 5) {
        panel->animations[i].flags &= ~1;
        panel->selected[i] = 0x2c;
    } else {
        panel->animations[i].flags &= ~1;
        panel->selected[i] = 0x2b;
    }
}
