#include "global.h"
#include "large_scene_animation_controller.h"

// Tentative interpretation: bit 0 hides the part visual; collider bit 2 disables it.

void ExplosionEffect_Create(void *, u32, u32, u32, u32);

static inline void AddPosition(const struct ContactPosition *offset,
                               struct ContactPosition *position)
{
    position->x = position->x + offset->x;
    position->y = position->y + offset->y;
    position->z = position->z + offset->z;
}

void Nidhoggr_HidePartWithExplosion(struct Nidhoggr *boss, s32 index)
{
    struct ContactPosition effect;
    u8 phase = boss->rowInitFlag[index];
    if (phase == 0) {
        boss->rows[index].flags |= 1;
        Nidhoggr_DisablePartCollider(&boss->colliders[index]);
        Nidhoggr_SetPartExtraFlags(&boss->partExtra[index], 1);
        effect.x = 0x100;
        effect.y = 0x80;
        effect.z = 0;
        AddPosition(&boss->rows[index].position, &effect);
        ExplosionEffect_Create(&effect, 0x10, 0, 0, phase);
        boss->rowDoneFlag[index] = 1;
        Nidhoggr_ChangeCompletedPartState(boss, index, 0);
    }
}
