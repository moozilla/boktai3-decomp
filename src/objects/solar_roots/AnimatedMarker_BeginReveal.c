#include "objects/solar_roots/animated_marker_collider_layout.h"
#define AnimatedMarker_RegisterEmptyCell SolarRoot_RegisterCellEffect
#define AnimatedMarker_TickReveal        SolarRoot_UpdateIdle
#include "global.h"
/* Tentative proximity-marker family; collider extent interpretation is provisional. */
struct AnimatedMarker {
    u32 flags;
    u8 pad4[3];
    u8 mode;
    u8 pad8[8];
    u16 timer;
    u8 pad12[0x48 - 0x12];
    struct AnimatedMarkerCollider collider;
    u8 pad54[0xf4 - 0x54];
    u16 phaseTimer;
    u16 padf6;
    u16 animationTimer;
    u8 padfa[8];
    u8 state;
    u8 pad103;
    u8 cellRef[0x10];
    void (*update)(struct AnimatedMarker *);
};
void TerrainModifiers_UnregisterCell(void *);
void SolarRoot_RegisterCellEffect(struct AnimatedMarker *);
void SolarRoot_UpdateIdle(struct AnimatedMarker *);
static inline void AnimatedMarker_SetUpdate(struct AnimatedMarker *marker,
                                            void (*update)(struct AnimatedMarker *))
{
    marker->update = update;
    marker->phaseTimer = 0;
    marker->state = 0;
}
void AnimatedMarker_BeginReveal(struct AnimatedMarker *marker)
{
    struct AnimatedMarkerCollider *collider;
    marker->flags &= ~0x200;
    marker->animationTimer = 0;
    marker->state = 0;
    marker->mode = 3;
    TerrainModifiers_UnregisterCell(marker->cellRef);
    SolarRoot_RegisterCellEffect(marker);
    collider = &marker->collider;
    collider->extent = 16;
    marker->timer = 0;
    AnimatedMarker_SetUpdate(marker, SolarRoot_UpdateIdle);
}
