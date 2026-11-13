#ifndef GUARD_PLAYER_CORE_H
#define GUARD_PLAYER_CORE_H
#include "engine.h"
#include "player.h"
#include "contact_actor.h"
#include "scene_object.h"
/* Script argument header is a packed count word; upper bits are preserved. */
struct PlayerEventArguments {
    u32 header;
    void *values;
};
struct PlayerEventCall {
    u32 value;
    struct PlayerEventArguments arguments;
};
struct DirectionalEffectFrame {
    u8 frame, flipX, flipY, unknown3;
};
extern const struct DirectionalEffectFrame gUnk_086108F8[];
void Anim_SetFrameDurationScale(void *, u32);
s32 AreaEffect_TestPlayerCoverage(u8 *);
s32 SunClock_GetPhase(void);
s32 ActorDirection_ComputeRelativeCardinal(struct ContactCollider *, struct ContactCollider *);
s32 ActorFlags_GetResponseBits(void *);
s32 ActorResponse_Apply(struct Player *, s32);
s32 Player_GetMaxLife(struct Player *);
s32 Player_GetMaxEnergy(struct Player *);
void Player_RecalculateDerivedStats(struct Player *);
void Player_ClampStatsAndSyncStatus(struct Player *);
void Player_InitializeStatsFromScript(struct Player *);
void Player_RefreshBodyCollision(struct Player *);
void PlayerAction_RefreshModifiers(void *);
void Collider_SetBodyProperties(void *, u16, u32, u32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void ParticleSprite_Initialize(void *, void *, s32);
void Effect_SetOrigin(void *, s32, s32);
void Player_SetDirectionalEffectFrame(struct PlayerEffectSprite *, void *, u32);
#endif
