#include "scene_actor_palette.h"
#define SceneActorPalette_IsTransitionComplete sub_08072124

static inline u8 Test(u32 v, u32 mask)
{
    if (v & mask)
        return TRUE;
    return FALSE;
}

static inline u8 TestP(u32 mask, u32 *p)
{
    if (*p & mask)
        return TRUE;
    return FALSE;
}

u32 SceneActorPalette_IsTransitionComplete(void)
{
    u32 id;
    struct SceneActorPaletteManagerView *manager = (void *)gUnk_020004A8;
    if (manager == NULL || !TestP(0x2000, &manager->flags))
        return 1;
    if (Test(gUnk_030053F4, 0x4800))
        id = 10;
    else {
        if (sub_080703F0(10))
            return 0;
        id = 23;
    }
    if (sub_080703F0(id))
        return 0;
    return 1;
}
