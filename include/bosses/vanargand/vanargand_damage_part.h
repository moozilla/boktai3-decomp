#ifndef GUARD_VANARGAND_DAMAGE_PART_H
#define GUARD_VANARGAND_DAMAGE_PART_H

#include "engine/video/display_sequence.h"
#include "engine/video/sprite.h"
#include "player/target_registry/target_registry.h"
#include "scene/collision/contact_collider.h"

struct VanargandDamagePart {
    struct DisplaySequenceSprite sprite;
    struct SpriteRenderResource graphics;
    struct DisplaySequenceState sequence;
    const void *sequenceResource;
    u32 sequenceId;
    u8 auxiliarySprite[0xA8 - 0x60];
    struct ContactCollider collider;
    u16 damage, contactFlags;
    u32 flags;
    u16 animation, breaks;
    u8 unknown108[8];
    struct MotionTarget target;
    s16 targetActive;
    u8 unknown122[2];
    u32 pendingFlags;
    u16 pendingDamage, accumulatedDamage, flashTimer;
    u16 contactCooldown;
    u16 *thresholds;
    s32 *mode;
};

void Vanargand_ApplyPartDamage(struct VanargandDamagePart *);

typedef char VanargandDamagePartSizeCheck[sizeof(struct VanargandDamagePart) == 0x138 ? 1 : -1];

#endif
