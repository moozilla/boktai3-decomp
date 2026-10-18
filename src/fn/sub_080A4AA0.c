#include "global.h"

void Sprite_Remove(void *);
void Anim_Remove(void *);
void Heap_Free(void *);
void ScaledShadow_Destroy(void *);
void TerrainOverlay_QueueRemoveActor(void *);
void SceneBody_Unregister(void *);
void Collider_Remove(void *);
void SceneActorRegistry_UnlinkActor(void *);

s32 Spider_Destroy(u8 *p)
{
    u8 *gfx = *(u8 **)(p + 4);
    if (!p[8])
        Sprite_Remove(gfx);
    else
        Anim_Remove(gfx + 0x20);
    Heap_Free(gfx);
    if (p[0x360])
        ScaledShadow_Destroy(p + 0x360);
    TerrainOverlay_QueueRemoveActor(p + 0x50);
    SceneBody_Unregister(p + 0x50);
    Collider_Remove(p + 0x134);
    SceneActorRegistry_UnlinkActor(p);
    return 0;
}
