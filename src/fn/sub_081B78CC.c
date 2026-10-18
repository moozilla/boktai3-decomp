#define AnimatedMarker_BeginActive          sub_081B78CC
#define AnimatedMarker_RegisterOccupiedCell sub_081B769C
#define AnimatedMarker_TickActive           sub_081B79A4
#include "global.h"
/* Tentative proximity-marker family; collider extent interpretation is provisional. */
struct AnimatedMarkerCollider {
    u8 pad0[6];
    u16 flags;
    u16 pad8;
    u16 extent;
};
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
void sub_081B769C(struct AnimatedMarker *);
void sub_081B79A4(struct AnimatedMarker *);
static inline void AnimatedMarker_SetUpdate(struct AnimatedMarker *marker,
                                            void (*update)(struct AnimatedMarker *), u8 state)
{
    marker->update = update;
    marker->phaseTimer = 0;
    marker->state = state;
}
void sub_081B78CC(struct AnimatedMarker *marker)
{
    struct AnimatedMarkerCollider *collider;
    marker->state = 1;
    TerrainModifiers_UnregisterCell(marker->cellRef);
    sub_081B769C(marker);
    collider = &marker->collider;
    collider->extent = 32;
    collider->flags |= 4;
    AnimatedMarker_SetUpdate(marker, sub_081B79A4, 1);
}
