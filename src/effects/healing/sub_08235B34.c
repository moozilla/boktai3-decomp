#include "effects/healing/healing_effect.h"
#include "system/game_state/game_state.h"
#include "scene/collision/packed_contact_vector.h"

void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void sub_08235B34(struct HealthBurstEffect *effect)
{
    struct PackedContactVector size, offset;
    struct ContactCollider *collider = &effect->collider;
    s32 amount;
    u32 flags, attackFlags;

    size.x = 30000;
    size.y = 30000;
    size.z = 30000;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    if (effect->mode == 0) {
        flags = 1 << effect->owner->movementRequestBlocked;
        amount = effect->owner->maximumStatA - effect->owner->currentStatA;
        attackFlags = 0x1000;
    } else {
        s32 level;

        if (effect->owner->actionMode == 6) {
            level = gGameState->level + 10;
            if (level > 99)
                level = 99;
        } else {
            level = gGameState->level;
        }
        flags = 0;
        amount = level * 8;
        attackFlags = 0x1000;
    }
    Collider_Init(collider, 0, 0x3001, 0, (u16)flags, &size, &offset);
    Collider_SetAttackProperties(collider, amount, 10, 0, attackFlags, 30);
    Collider_SetCallback(collider, 0, effect);
    Collider_SetPosition(collider, &effect->owner->position, 0);
}
