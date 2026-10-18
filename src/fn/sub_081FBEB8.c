#define OverlaySlot_InitializeExtended sub_081FBEB8
#define SlotMotion_InitializeExtended Ratatosk_InitAnimatedParticleParams
#include "global.h"
#include "terrain_overlay.h"
struct ExtendedOverlaySlot {
    u32 flags;
    u8 pad4[8];
    u32 position[2];
    u8 motion[0x1c];
    struct TerrainOverlayDrawable drawable;
    u8 pad5C[4];
    u32 field60, field64;
};
struct OverlayManager {
    u8 task[0x1c];
    u8 animation[0x1c];
};
void Ratatosk_InitAnimatedParticleParams(void *);
void Sprite_InitUnlinked(struct TerrainOverlayDrawable *, void *, s32);
static inline void clearPosition(void *target, u32 *source, u32 flags)
{
    *source = 0;
    CpuSet(source, target, flags);
}
void sub_081FBEB8(struct OverlayManager *manager, struct ExtendedOverlaySlot *slot)
{
    u32 zero;
    Ratatosk_InitAnimatedParticleParams(slot->motion);
    slot->flags = 0;
    slot->field60 = 0;
    slot->field64 = 0;
    clearPosition(slot->position, &zero, 0x05000002);
    Sprite_InitUnlinked(&slot->drawable, manager->animation, 1);
}
