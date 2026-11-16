#include "global.h"
#include "large_scene_animation_controller.h"

// Tentative interpretation: bit 0 hides the part visual; collider bit 2 disables it.

struct NidhoggrPosition {
    u16 x, y, z;
};

struct NidhoggrEffectPosition {
    u32 x : 16, y : 16, z : 16;
};

void ExplosionEffect_Create(void *, u32, u32, u32, u32);

static inline void AddPosition(struct NidhoggrPosition *offset, struct NidhoggrPosition *position)
{
    position->x = position->x + offset->x;
    position->y = position->y + offset->y;
    position->z = position->z + offset->z;
}

void Nidhoggr_HidePartWithExplosion(struct Nidhoggr *boss, s32 index)
{
    struct NidhoggrEffectPosition effect;
    struct NidhoggrPosition *position;
    struct NidhoggrPosition *offset;
    u8 phase = boss->rowInitFlag[index];
    if (phase == 0) {
        boss->rows[index].flags |= 1;
        ((struct NidhoggrCollider *)((u8 *)boss + index * 0x54 + 0x66D8))->flags |= 4;
        ((struct NidhoggrPartExtra *)((u8 *)boss + index * 0x10 + 0x6F60))->flags |= 1;
        effect.x = 0x100;
        effect.y = 0x80;
        effect.z = 0;
        position = (struct NidhoggrPosition *)&effect;
        offset = (struct NidhoggrPosition *)(index * 0x60 + (u32)boss + 0x545C);
        AddPosition(offset, position);
        ExplosionEffect_Create(position, 0x10, 0, 0, phase);
        boss->rowDoneFlag[index] = 1;
        Nidhoggr_ChangeCompletedPartState(boss, index, 0);
    }
}
