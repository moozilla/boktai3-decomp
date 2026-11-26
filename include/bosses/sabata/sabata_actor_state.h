#ifndef GUARD_BOSSES_SABATA_SABATA_ACTOR_STATE_H
#define GUARD_BOSSES_SABATA_SABATA_ACTOR_STATE_H
#include "engine/video/anim_sprite.h"
#include "scene/actor/scene_actor_components.h"
#include "scene/camera/camera_tracking.h"
#include "scene/collision/contact_collider.h"
#include "bosses/sabata/converging_particles.h"
/* Actor state shared by Sabata’s combat and presentation callbacks. */
struct SabataActorState {
    u8 unknown0[0x48];
    struct SceneActorBody body;
    u32 stateTicks;
    s16 vitality;
    u8 unknown96[4];
    u8 checking;
    u8 unknown9B;
    u32 flags;
    u8 unknownA0[2];
    u8 stateReset, unknownA3;
    u32 pendingAction;
    u8 facing4, facing8;
    u8 state;
    u8 controlPhase;
    u8 active, phase, variant, pending, triggered;
    u8 changed, finished, special;
    u8 unknownB4[8];
    u32 rewardMask, contactAttributes;
    u32 timer;
    u32 threshold;
    u8 requestedDirection;
    u8 unknownCD[0x17];
    u32 presentationFrames;
    u8 unknownE8[0x18];
    void *stateCallback;
    void *callback;
    u8 unknown108[4];
    void *damageTarget;
    struct ContactPosition *selectedPoint;
    s8 alternateDirection;
    u8 unknown115[0x150 - 0x115];
    s16 controlPhaseTimer;
    u8 unknown152[0x178 - 0x152];
    struct ContactCollider attackCollider, bodyCollider, proximityCollider;
    u8 unknown274[0x288 - 0x274];
    u8 bounds[0x1C];
    struct ContactPosition destination;
    struct AnimSpriteResource animationResource;
    struct AnimSprite graphics;
    u8 effect[0xB0], shadow[0x40];
    void *frostEmitter;
    u8 unknown420[8];
    u8 proximityVolume[0x40];
    s16 height;
    u16 lastDamage;
    u32 hitAttributes;
    u8 slowDown, direction, hitParameter, impactCount;
    u8 unknown474[3];
    u8 transitionBlocked;
    struct ContactPosition savedPosition;
    struct ContactPosition nearbyPosition;
    u8 unknown488[0x58];
    u8 animationState, unknown4E1, animationPhase, animationIndex, animationDetail, mode;
    u16 decisionTimeout;
    u8 request, requestedVariant, requestMode, requestPhase;
    s16 paletteRestoreTimer;
    u16 contactHoldoff;
    u8 unknown4F0[2];
    s16 floorHeight;
    s16 attackFlags;
    u8 unknown4F6[6];
    s16 hazardParameter;
    u8 unknown4FE[6];
    s8 paletteTimer;
    u8 unknown505, paletteMode, paletteShift;
    u8 unknown508[8];
    s8 randomOffset;
    u8 jitterTicks;
    u8 unknown512[0xC];
    u8 emitBursts, shakeTimer;
    struct ConvergingParticleGroup convergingParticles;
    struct ContactPosition offsetPoint;
    u16 effectPalettes[3][256];
    u16 paletteData[16];
    struct ContactPosition movementTarget;
    u8 unknownC28[8];
    struct CameraTracking camera;
    u32 summons[2];
};

typedef char SabataActorStateSizeCheck[sizeof(struct SabataActorState) == 0xC68 ? 1 : -1];

/* The owner retains this actor prefix at offset zero. */
struct SabataActorStateOwner {
    struct SabataActorState actor;
};
#endif
