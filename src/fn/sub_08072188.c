#include "scene_actor_palette.h"
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return TRUE;
    return FALSE;
}
static inline u8 TestP(u32 mask, u32 *field)
{
    if (*field & mask)
        return TRUE;
    return FALSE;
}
static inline void KeepFlags(u32 mask, u32 *field)
{
    *field = mask & *field;
}
void SceneActorPalette_ClearCompletedTransitions(void)
{
    struct SceneActorPaletteManagerView *manager = (void *)gUnk_020004A8;
    s32 pending;
    if (!manager || !TestP(0x2000, &manager->flags))
        return;
    if (Test(gEngineFlags, 0x4800)) {
        pending = SceneActorRegistry_CountLivingByCategory(10);
    } else {
        if ((u8)SceneActorRegistry_TestCategoryFlags(10, 0x20000))
            return;
        pending = SceneActorRegistry_TestCategoryFlags(23, 0x20000) << 24;
    }
    if (!pending)
        KeepFlags(~0x22000, &manager->flags);
}
