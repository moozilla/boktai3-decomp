#ifndef GUARD_BOSSES_COUNT_COUNT_H
#define GUARD_BOSSES_COUNT_COUNT_H

#include "engine/video/anim_sprite.h"
#include "scene/actor/scene_actor_components.h"
#include "scene/camera/camera_tracking.h"
#include "scene/collision/contact_collider.h"

struct CountPaletteBlend {
    u32 amount : 8;
    u32 descending : 8;
    u32 mode : 8;
    u32 shift : 8;
    s32 resource;
};

struct Count {
    u8 unknown0[0x48];
    struct SceneActorBody body;
    u32 callbackFrame;
    s16 health, maxHealth;
    u8 unknown98[2];
    u8 healthCheck, unknown9B;
    u32 flags;
    u8 unknownA0[0x13];
    u8 ambientMode;
    u8 unknownB4[8];
    u32 contactAttack, contactFlags;
    u8 unknownC4[0x20];
    u32 effectTimer;
    u8 unknownE8[0x90];
    struct ContactCollider bodyCollider, overlayCollider;
    struct AnimSpriteResource animationResource;
    struct AnimSprite animation;
    u8 shadow[0x6C];
    u8 unknown30C[0x30];
    s32 pendingDamage;
    u8 pendingSpecial, forcedHit, hitAngle, variant;
    u8 unknown344[8];
    u16 paletteRestoreTimer, invulnerabilityTimer;
    u16 effectCooldown;
    u8 unknown352[6];
    s32 graphicsKind;
    u8 unknown35C[0x1A];
    s16 hitThreshold;
    u8 unknown378[0xC];
    u8 damageGuard;
    u8 unknown385[7];
    struct CountPaletteBlend blend;
    u8 unknown394[4];
    u8 shakeAngle, shakeAmplitude;
    u8 unknown39A[2];
    s32 queuedHeal, queuedDamage;
    u8 unknown3A4[4];
    u32 hitCount, damageFlags;
    u8 unknown3B0[8];
    u8 ambientParticles;
    u8 unknown3B9[7];
    s32 backgroundFlashTimer;
    u16 paletteDefault[0x100];
    u16 paletteNormal[0x100];
    u16 paletteSpecial[0x100];
    u8 unknown9C4[0x1C];
    struct CameraTracking camera;
};

typedef char CountSizeCheck[sizeof(struct Count) == 0xA10 ? 1 : -1];

#endif
