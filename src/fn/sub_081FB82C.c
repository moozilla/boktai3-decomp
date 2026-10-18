#include "global.h"
#include "terrain_overlay.h"
struct OverlaySlot {
    u32 flags;
    u8 pad4[8];
    u32 position[2];
    u8 animation[0x18];
    struct TerrainOverlayDrawable drawable;
    u8 pad58[4];
    u32 field5C, field60;
    u8 tail[0x10];
};
struct OverlayManager {
    u8 task[0x1c];
    u8 animation[0x1c];
};
void SlotMotion_Initialize(void *);
void Sprite_InitUnlinked(struct TerrainOverlayDrawable *, void *, s32);
static inline void clearPosition(void *target, u32 *source, u32 flags)
{
    *source = 0;
    CpuSet(source, target, flags);
}
void OverlaySlot_Initialize(struct OverlayManager *manager, struct OverlaySlot *slot)
{
    u32 zero;
    SlotMotion_Initialize(slot->animation);
    slot->flags = 0;
    slot->field5C = 0;
    slot->field60 = 0;
    clearPosition(slot->position, &zero, 0x05000002);
    Sprite_InitUnlinked(&slot->drawable, manager->animation, 1);
}
