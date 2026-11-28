#include "global.h"
#include "minigames/result_game/result_tutorial.h"
int ResultMenu_ReadScript(void *);
void ResultMenu_CreateGraphics(void *);
void ResultMenu_SetPhase(void *, void *);
void ResultMenu_UpdateIntro(void *);
int ResultMenu_Initialize(struct ResultMenu *menu, void *owner)
{
    struct AnimSpriteResource *resource;
    menu->owner = owner;
    resource = ScriptProc_Start(0xcb05, 0x530d);
    if (!resource)
        return -1;
    menu->primaryResource = *resource;
    ResourceHeader_Relocate(&menu->primaryResource, resource);
    resource = ScriptProc_Start(0xcb05, 0x5d04);
    if (!resource)
        return -1;
    menu->secondaryResource = *resource;
    ResourceHeader_Relocate(&menu->secondaryResource, resource);
    if (ResultMenu_ReadScript(menu) < 0)
        return -1;
    ResultMenu_CreateGraphics(menu);
    ResultMenu_SetPhase(menu, ResultMenu_UpdateIntro);
    return 0;
}
