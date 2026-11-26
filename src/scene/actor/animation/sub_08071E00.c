#include "scene/actor/scene_actor_palette.h"
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return TRUE;
    return FALSE;
}
void SceneActorPalette_BeginTransition(u32 destinationBank, u32 sourceBank, u32 unused,
                                       s32 category, u32 auxiliaryFlag)
{
    struct SceneActorPaletteManagerView *manager = (void *)gSceneActorRegistryManager;
    struct SceneActorPaletteTransition *transition;
    const u16 *destination;
    const u16 *source;
    s32 i;
    u32 activeFlag;
    if (!manager)
        return;
    switch (category) {
        case 23:
            transition = &manager->category23;
            break;
        case 10:
            transition = &manager->category10;
            break;
        default:
            return;
    }
    SceneActorRegistry_FindLiving(category);
    destination = (const u16 *)(gPaletteBankData + destinationBank * 32);
    source = (const u16 *)(gPaletteBankData + sourceBank * 32);
    transition->destinationBank = destinationBank;
    activeFlag = 0x2000;
    for (i = 0; i < 16; i++) {
        s32 base;
        if (Test(manager->flags, activeFlag) && transition->remaining != 32)
            base = transition->colors[i];
        else
            base = source[i];
        transition->deltaRed[i] = (destination[i] & 31) - (base & 31);
        transition->deltaGreen[i] = ((destination[i] >> 5) & 31) - ((base >> 5) & 31);
        transition->deltaBlue[i] = ((destination[i] >> 10) & 31) - ((base >> 10) & 31);
        transition->red[i] = (base & 31) << 5;
        transition->green[i] = ((base >> 5) & 31) << 5;
        transition->blue[i] = ((base >> 10) & 31) << 5;
    }
    {
        u32 mask = 0x2000;
        manager->flags |= mask;
    }
    transition->remaining = 32;
    if (auxiliaryFlag) {
        u32 mask = 0x20000;
        manager->flags |= mask;
    } else {
        u32 mask = ~0x20000;
        manager->flags &= mask;
    }
}
