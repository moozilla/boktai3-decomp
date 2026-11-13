#ifndef GUARD_FOLLOWING_SPRITE_EFFECT_H
#define GUARD_FOLLOWING_SPRITE_EFFECT_H
#include "particle_effect_state.h"
#include "player_cue_sequence.h"
#include "scene_arc.h"

struct FollowingSpriteManager {
    u8 task[0x18];
    struct FollowingEffect entries[2];
    u32 frame;
};
struct FollowingSpriteParameters {
    u16 unknown0, duration, unknown4, value;
};
struct FollowingEffectCoordinate {
    u32 low : 16, high : 16;
};
struct FollowingEffectBounds {
    struct FollowingEffectCoordinate horizontal, vertical;
};

struct PositionLinkedSprite {
    u8 resource[0x1c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState animation;
    struct FollowingPosition *position;
    s32 *height;
    u32 active;
};
struct PositionLinkedSpriteManager {
    u8 task[0x18];
    struct PositionLinkedSprite entries[2];
    void *sequence;
};

union ScreenMotePosition {
    struct {
        s16 x, y, z, padding;
    } axis;
    u32 words[2];
};
struct ScreenMoteSprite {
    u32 flags;
    u8 unknown4[0xb], mode, unknown10[8];
    union ScreenMotePosition position;
    u8 unknown20[8];
};
struct ScreenMoteControl {
    u16 options;
    u8 phase;
    s8 spriteIndex;
    u16 age, unknown1A, remaining;
    u8 delay, entering;
    u32 active;
};
struct ScreenMoteState {
    union ScreenMotePosition position;
    u8 unknown8[8];
    union ScreenMotePosition *target;
    struct ScreenMoteControl control;
};
struct ScreenMoteManager {
    u8 task[0x18];
    void *resource;
    struct ScreenMoteSprite sprites[32];
    struct ScreenMoteState states[32];
};

void ParticleBurstEffect_ResetManager(void);
void FollowingSpriteEffect_SetStateCallback(struct FollowingEffect *,
                                            void (*)(struct FollowingEffect *));
void FollowingSpriteEffect_OnStop(struct FollowingEffect *);
void FollowingSpriteEffect_UpdateFollowing(struct FollowingEffect *);
s32 PairCallbackPool_Update(struct FollowingSpriteManager *);
u32 FollowingSpriteEffect_DestroyManager(void);
s32 SpritePairPool_Initialize(struct FollowingSpriteManager *);
struct FollowingSpriteManager *FollowingSpriteEffect_EnsureManager(void);
s32 PairCallbackPool_MarkNextOfKind(struct FollowingSpriteManager *, s32, s32, u8 *);
u8 SpritePairPool_TestUnavailable(struct FollowingSpriteManager *, u32, s32);
void FollowingSpriteEffect_OnAnimationComplete(void);
void SpritePairEntry_CreateEffect(struct FollowingEffect *, void (*)(void), u32, u32);
s32 SpritePairPool_StartEntry(struct FollowingPosition *, s32 *, u32,
                              struct FollowingSpriteParameters *, u32);
void SpritePair_ReleaseSlot(u32, u32);
void FollowingSpriteEffect_ResetManager(void);
s32 SpritePair_UpdateAnimation(struct PositionLinkedSpriteManager *);
s32 PositionLinkedSprite_DestroyManager(void);
s32 CompactSpritePool_Initialize(struct PositionLinkedSpriteManager *);
struct PositionLinkedSpriteManager *PositionLinkedSprite_EnsureManager(void);
s32 DisplaySlots_Reserve(struct FollowingPosition *, s32 *);
void SpritePair_RemoveSlot(u32);
void ActorMarker_ResetManager(void);
void ActorMarker_HideSlot(s32);
void ActorMarker_ShowSlot(s32);
void SpriteParticle_InitSystem(void);
void ScreenMote_ApproachTarget(struct ScreenMoteManager *, struct ScreenMoteState *);
void SpriteParticle_ReleaseSlot(struct ScreenMoteManager *, struct ScreenMoteState *);
struct ScreenMoteState *SpriteParticle_AllocateSlot(struct ScreenMoteManager *);
s32 ScreenMote_SpawnBatch(union ScreenMotePosition *, union ScreenMotePosition *, s16 *, s32, u32,
                          u32, u32, u32);
s32 SpriteParticle_SpawnSingleFrameBatch(union ScreenMotePosition *, union ScreenMotePosition *,
                                         s16 *, s32, u32, u32, u32);
s32 SpriteParticle_SpawnAnimatedBatch(union ScreenMotePosition *, union ScreenMotePosition *, s16 *,
                                      s32, u32, u32, u32);
s32 ScreenMote_UpdateManager(struct ScreenMoteManager *);
s32 EffectPool_Destroy(struct ScreenMoteManager *);
void SpriteParticle_LoadAnimationData(struct ScreenMoteManager *);
s32 SpriteParticle_InitManager(struct ScreenMoteManager *);
struct ScreenMoteManager *SpriteParticle_GetOrCreateManager(void);
#endif
