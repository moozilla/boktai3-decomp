#include "global.h"
#include "link/panel/link_panel.h"
void SceneMenu_InitializeDisplay(struct LinkPanel *p)
{
    s32 i;
    struct AnimSpriteResource *resource;
    resource = ScriptProc_Start(0xCB05, 0x1289);
    p->entrySource = resource;
    ResourceHeader_Relocate(&p->entryResource, resource);
    resource = ScriptProc_Start(0xCB05, 0x530D);
    ResourceHeader_Relocate(&p->lastEntryResource, resource);
    for (i = 0; i <= 5; i++) {
        Widget_InitLinked(&p->animations[i], &p->entryResource, p->selected[i], 0x10, 1, 0, 0, 0);
        p->previous[i] = -1;
    }
    p->animations[2].attribute1A = 0;
    p->animations[3].attribute1A = 0;
    Widget_InitLinked(&p->animations[6], &p->lastEntryResource, p->selected[6], 0x10, 0, 0, 0, 0);
    p->previous[6] |= -1;
}
