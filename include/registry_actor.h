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
    s8 boostEffect;
    u8 unknown8AB;
    s32 lastForwardSpeed;
    u32 ticks, active;
    u16 boostFrames, updateFlags;
    void *owner;
    u32 attachmentFlags, attachmentParameter;
    s16 attachmentScreenX, attachmentScreenY;
    u32 effectFlags, projectile;
    u8 unknown8D4[0xC];
    s8 direction;
    u8 unknown8E1[0xF];
    s16 attachmentOffsetX, attachmentOffsetY;
    u8 unknown8F4[0x10];
    void *attachment;
};
struct RegistryActor {
    struct ActorStats stats;
    u16 directionAngle;
    u8 unknown5F2[6];
    struct CharacterPreviewModel model; /* ends at 0x84C with its appearance bytes */
    u32 fallTimer;
    u8 resetKeys, unknown851[3];
    struct RegistryActorControl control;
};
enum {
    REGISTRY_ACTOR_SLOT_COUNT = 1,
    REGISTRY_ACTOR_MOTION_COMMANDS = 1,
    REGISTRY_ACTOR_MESSAGES_PAUSED = 0x800,
};

/* The ability callback owner is the task containing this one-slot registry. */
struct ActorAbilityOwner {
    struct Task task;
    struct RegistryActor actors[REGISTRY_ACTOR_SLOT_COUNT];
};
typedef struct ActorAbilityOwner RegistryActorManager;

static inline void RegistryActor_StartMotion(struct RegistryActor *actor,
                                             void (*update)(struct RegistryActor *))
{
    actor->resetKeys = 1;
    actor->control.update = update;
    actor->fallTimer = 0;
}

static inline void RegistryActor_SetMotionFlags(u16 bits, struct RegistryActorMotion *motion)
{
    motion->flags = bits | motion->flags;
}

static inline u32 RegistryActor_MessagePauseBits(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}

struct RegistryActor *RegistryA8CB_FindOwnedEntry(void *);
void ActorMotion_UpdateFallFrame(struct RegistryActor *);
s32 SceneActor_DispatchCommands(struct RegistryActor *);
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
