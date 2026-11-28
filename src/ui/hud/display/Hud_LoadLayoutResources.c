#include "global.h"
#include "ui/pause/scene_hud_display.h"
struct Header {
    u32 words[8];
};
void ParticleSprite_Initialize(void *, const void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Hud_LoadLayoutResources(struct SceneHudState *hud)
{
    struct Header *resource;
    s32 i;
    resource = ScriptProc_Start(0xcb05, 0x530d);
    if (!resource)
        return;
    *(struct Header *)hud->widgetResource = *resource;
    ResourceHeader_Relocate(hud->widgetResource, resource);
    resource = ScriptProc_Start(0xcb05, 0x5d04);
    if (!resource)
        return;
    *(struct Header *)hud->motionResource = *resource;
    ResourceHeader_Relocate(hud->motionResource, resource);
    resource = ScriptProc_Start(0xcb05, 0xa2db);
    if (!resource)
        return;
    *(struct Header *)(hud->widgetResource + 0x20) = *resource;
    ResourceHeader_Relocate(hud->widgetResource + 0x20, resource);
    resource = ScriptProc_Start(0xcb05, 0x32ba);
    if (!resource)
        return;
    *(struct Header *)&hud->accountResource = *resource;
    ResourceHeader_Relocate(&hud->accountResource, resource);
    resource = ScriptProc_Start(0xcb05, 0xde23);
    if (!resource)
        return;
    *(struct Header *)hud->unknownE4 = *resource;
    ResourceHeader_Relocate(hud->unknownE4, resource);
    resource = ScriptProc_Start(0xcb05, 0xdfde);
    if (!resource)
        return;
    *(struct Header *)hud->actionResultResources[0] = *resource;
    ResourceHeader_Relocate(hud->actionResultResources[0], resource);
    resource = ScriptProc_Start(0xcb05, 0xe2de);
    if (!resource)
        return;
    *(struct Header *)hud->actionResultResources[1] = *resource;
    ResourceHeader_Relocate(hud->actionResultResources[1], resource);
    resource = ScriptProc_Start(0xcb05, 0x7c8b);
    if (!resource)
        return;
    *(struct Header *)hud->controlAnimationResource = *resource;
    ResourceHeader_Relocate(hud->controlAnimationResource, resource);
    resource = ScriptProc_Start(0xcb05, 0x2bec);
    if (!resource)
        return;
    *(struct Header *)hud->comparisonResource = *resource;
    ResourceHeader_Relocate(hud->comparisonResource, resource);
    hud->numberDigitResource = AnimData_FindEntry(0x1c1c);
    for (i = 0; i < 4; i++) {
        ParticleSprite_Initialize(&hud->numberDigits[i], hud->numberDigitResource, 128);
        Effect_SetOrigin(&hud->numberDigits[i], -4, -4);
        hud->numberDigits[i].flags |= 17;
        hud->numberDigits[i].priority = 0;
    }
}
