#include "global.h"

// Tentative interpretation: bit 0 hides the part visual; collider bit 2 disables it.

struct NidhoggrPosition {
    u16 x, y, z;
};

struct NidhoggrEffectPosition {
    u32 x : 16, y : 16, z : 16;
};

struct NidhoggrPartVisual {
    u32 flags;
    u8 unknown04[0x18 - 4];
    struct NidhoggrPosition position;
    u8 unknown20[0x60 - 0x20];
};

struct NidhoggrPartCollider {
    u8 unknown00[6];
    u16 flags;
    u8 unknown08[0x54 - 8];
};

struct NidhoggrPartAuxiliary {
    u32 unknown00;
    u32 flags;
    u8 unknown08[8];
};

struct Nidhoggr {
    u8 unknown0000[0x5444];
    struct NidhoggrPartVisual parts[26];
    u8 unknown5E04[0x66D8 - 0x5E04];
    struct NidhoggrPartCollider colliders[26];
    struct NidhoggrPartAuxiliary auxiliary[26];
    u8 unknown7100[0x71DC - 0x7100];
    u8 partPhases[26];
    u8 unknown71F6[26];
    u8 partCompleted[26];
};

void ExplosionEffect_Create(void *, u32, u32, u32, u32);
s32 Nidhoggr_ChangeCompletedPartState(struct Nidhoggr *, s32, u32);

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
    u8 phase = boss->partPhases[index];
    if (phase == 0) {
        boss->parts[index].flags |= 1;
        ((struct NidhoggrPartCollider *)((u8 *)boss + index * 0x54 + 0x66D8))->flags |= 4;
        ((struct NidhoggrPartAuxiliary *)((u8 *)boss + index * 0x10 + 0x6F60))->flags |= 1;
        effect.x = 0x100;
        effect.y = 0x80;
        effect.z = 0;
        position = (struct NidhoggrPosition *)&effect;
        offset = (struct NidhoggrPosition *)(index * 0x60 + (u32)boss + 0x545C);
        AddPosition(offset, position);
        ExplosionEffect_Create(position, 0x10, 0, 0, phase);
        boss->partCompleted[index] = 1;
        Nidhoggr_ChangeCompletedPartState(boss, index, 0);
    }
}
