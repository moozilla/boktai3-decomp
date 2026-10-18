#include "global.h"
#include "terrain_overlay.h"
#define SceneOverlayConfig_Init EnemyParticle_InitParameters
struct SceneOverlayConfig {
    u8 unknown0[0x14];
    u32 resource;
    u8 unknown18[0x20];
};
struct SceneOverlayEntry {
    u32 flags;
    u8 unknown4[0xC];
    u32 position[2];
    u8 unknown18[0x10];
    struct SceneOverlayConfig config;
    u8 animation[0x1C];
    struct TerrainOverlayDrawable drawable;
};
void EnemyParticle_InitParameters(struct SceneOverlayConfig *);
void Sprite_InitUnlinked(struct TerrainOverlayDrawable *, void *, s32);
static inline void ClearPosition(void *dest)
{
    u32 zero = 0;
    CpuSet(&zero, dest, 0x05000002);
}
void SceneOverlayEntry_Init(void *owner, struct SceneOverlayEntry *entry, s32 index)
{
    struct SceneOverlayConfig *config = &entry->config;
    void *animation;
    EnemyParticle_InitParameters(config);
    entry->flags = 0;
    ClearPosition(entry->position);
    animation = entry->animation;
    AnimData_Load(animation, config->resource);
    Sprite_InitUnlinked(&entry->drawable, animation, 1);
}
