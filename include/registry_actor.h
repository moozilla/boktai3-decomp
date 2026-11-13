#ifndef GUARD_REGISTRY_ACTOR_H
#define GUARD_REGISTRY_ACTOR_H
#include "engine.h"
#include "actor_stats.h"
#include "modifier_list.h"

struct RegistryActorMotion {
    s32 x, y, z;
    u8 unknownC[4];
    u8 parameterB, parameterD;
    u16 remaining;
    u8 unknown14[2];
    u16 flags;
};
struct RegistryActor;
struct RegistryActorControl {
    void (*update)(struct RegistryActor *);
    u8 messages[0x34];
    struct RegistryActorMotion motion;
    u8 unknown8A4[6];
    s8 keyframe;
    u8 unknown8AB;
    s32 lastForwardSpeed;
    u32 ticks, active;
    u16 unknown8B8, updateFlags;
    void *owner;
    u8 unknown8C0[0xC];
    u32 effectFlags, projectile;
    u8 unknown8D4[0xC];
    s8 direction;
    u8 unknown8E1[0x27];
};
struct RegistryActor {
    struct ActorStats stats;
    u16 directionAngle;
    u8 unknown5F2[6];
    struct CharacterPreviewModel model;
    u8 unknown848[4];
    u32 fallTimer;
    u8 resetKeys, unknown851[3];
    struct RegistryActorControl control;
};
/* The ability callback owner is the task containing this one-slot registry. */
struct ActorAbilityOwner {
    struct Task task;
    struct RegistryActor actors[1];
};
typedef struct ActorAbilityOwner RegistryActorManager;

struct RegistryActor *RegistryA8CB_FindOwnedEntry(void *);
void ActorMotion_UpdateFallFrame(struct RegistryActor *);
void SceneActor_DispatchCommands(struct RegistryActor *);
void Character_UpdateTimedEffect(void *);
void Anim_Remove(void *);
void SpatialIndex_RemoveEntry(void *);
void SceneMessages_UnregisterReceiver(void *);
s32 Callback_InvokeActor(void *, void *);
RegistryActorManager *RegistryA8CB_EnsureManager(void);
struct RegistryActor *RegistryA8CB_FindAvailableEntry(RegistryActorManager *);
struct RegistryActor *RegistryA8CB_FindEntryByName(RegistryActorManager *, u32);
void ActorScript_ParseParams(struct RegistryActor *);
void ActorSprite_InitPartDrawings(struct RegistryActor *, void *, s32);
void CharacterPreview_InitializePalette(void *, void *, u8);
void SceneSlot_RegisterObject(void *);
void ActorCollision_InitBounds(struct RegistryActor *);
void RegistryA8CB_RegisterMessageReceiver(struct RegistryActor *);
void Modifier_InitDrainState(struct ActorDrainState *, void *, u16, u8);
void ActorPhysics_UpdateFallMotion(struct RegistryActor *);
void SceneMessages_RegisterReceiver(void *, u32, u32);
void RegistryA8CB_SetKeyframes(struct RegistryActor *, u32);
s32 MotionTween_Advance(struct RegistryActor *, void *);
void Character_UpdateLoopingSound(struct RegistryActor *);
s32 Div(s32, s32);
void Terrain_ResolveActorMotion(struct RegistryActor *);
void PalFade_Update(struct RegistryActor *);
void Actor_UpdateShadowSpriteParts(struct RegistryActor *, void *);
void ActorParticles_SpawnImpactBurst(struct RegistryActor *);
void CharacterModel_UpdateAccessoryAnimations(void *);
#endif
