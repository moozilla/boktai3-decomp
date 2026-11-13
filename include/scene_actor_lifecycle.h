#ifndef GUARD_SCENE_ACTOR_LIFECYCLE_H
#define GUARD_SCENE_ACTOR_LIFECYCLE_H

#include "engine.h"
#include "player.h"
#include "scene_actor_components.h"
#include "scene_body_api.h"
#include "display_animation_api.h"

struct SceneActorResourceEntry {
    u16 type;
    u8 variant, unknown3;
    void *data;
};
struct SceneActorAnimationEntry {
    u8 kind;
    s8 stride;
    u16 animation;
};
/* The display wrapper overlays a sprite resource or a keyframe resource header. */
struct SceneActorSequenceResource {
    u8 unknown0[6];
    u16 paletteIndex;
    u8 unknown8[4];
    const void *paletteData;
    u8 unknown10[0xC];
};
struct SceneActorSequenceDisplay {
    u32 kind, index;
    u8 variant, flags, finished, unknownB;
    struct SceneActorSequenceResource resource;
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState sequence;
    void *sequenceResource;
    u8 unknown68[0xC];
    const void *keyframeTileSource;
    u8 unknown78[0x14];
    struct DisplayPosition *position;
    u32 *displayFlags;
};
struct SceneActorKeyframeDisplay {
    u32 kind, index;
    u8 variant, flags, finished, unknownB;
    u32 resourceHeader[8];
    struct DisplayKeyframeState keyframes;
    u8 unknown68[0xC];
    const void *keyframeTileSource;
    u8 unknown78[0x14];
    struct DisplayPosition *position;
    u32 *displayFlags;
};
union SceneActorDisplay {
    struct DisplayAnimation common;
    struct SceneActorSequenceDisplay sequence;
    struct SceneActorKeyframeDisplay keyframe;
};
/* Both shadow kinds reserve this block; the callbacks choose its interpretation. */
struct SceneActorShadow {
    u8 state[0x6C];
};
struct SceneActor {
    u8 active, variant;
    u8 unknown2[0x19];
    u8 direction;
    u16 defaultPaletteBank, paletteBank;
    u8 unknown20[0x3C];
    struct SceneActorBody body;
    struct ContactPosition positionOffset;
    u8 unknownAC[0x28];
    union SceneActorDisplay drawable;
    u8 unknown168[0x8A];
    u8 motionRegistered;
    u8 unknown1F3[0x25];
    u8 shadowKind;
    u8 unknown219[0x2B];
    struct SceneActorShadow shadow;
    u8 unknown2B0[8];
    const struct SceneActorAnimationEntry *animationEntries;
    const struct SceneActorResourceEntry *resourceEntry;
    u8 unknown2C0[0x10];
    void (*callbacks[4])(void *, struct SceneActor *);
    void (*destroy)(void *, struct SceneActor *);
    u8 unknown2E4[4];
    struct SceneActor *next, *previous;
    u8 unknown2F0[0x94];
};
struct SceneActorManager {
    struct Task task;
    u32 ticks;
    struct SceneActor *head;
    u8 minuteChanged, previousMinute, hour, minute, weather;
    u8 unknown25[3];
    u32 buffer, count[2];
    u8 unknown34[0x400];
    struct ContactPosition *target;
    struct Player *player;
};

extern const struct SceneActorResourceEntry gUnk_086053D4[], gUnk_086054D4[];
void *Actor_FindResourceData(int, int);
const struct SceneActorResourceEntry *Actor_FindResourceEntry(int, int);
int Actor_InitResources(void *, struct SceneActor *, int, int);
void Actor_LinkToManager(struct SceneActorManager *, struct SceneActor *);
void Actor_UnlinkFromManager(struct SceneActorManager *, struct SceneActor *);
struct SceneActor *Actor_FindById(struct SceneActorManager *, int);
struct SceneActor *Actor_Allocate(int, int);
int Actor_InitMotion(struct SceneActor *, int, const struct ContactPosition *, int);
int Actor_InitDrawable(struct SceneActor *, int, int, int, int);
int Actor_InitShadow(struct SceneActor *, int, int, int, int);
u32 Actor_DestroyShadow(struct SceneActor *);
u32 Actor_DisableShadow(struct SceneActor *);
u32 Actor_EnableShadow(struct SceneActor *);
int Actor_RegisterMotion(struct SceneActor *);
int Actor_UnregisterMotion(struct SceneActor *);
u32 Actor_SetPaletteBank(struct SceneActor *, u32);
u32 Actor_SetDefaultPaletteBank(struct SceneActor *, u16);
s32 Actor_ResetPaletteBank(struct SceneActor *);
int Actor_Destroy(struct SceneActor *);
int Actor_RunCallbacks(void *, struct SceneActor *);

int SceneBody_SetPositionTarget(void *, void *);
void SceneBody_SetKeyframeDisplay(void *, void *);
void SceneBody_Unregister(void *);
int DisplayAnimation_SetPriority(void *, int);
void Shadow_Initialize(void *, int, int, int);
void Shadow_Destroy(void *);
void ScaledShadow_Initialize(void *, int, int, int, int, int, int, int, int, int, int);
void ScaledShadow_SetGraphic(void *, int, int);
void ScaledShadow_Enable(void *);
void ScaledShadow_Disable(void *);
void ScaledShadow_Destroy(void *);
int TerrainOverlay_QueueAddActor(void *);
void TerrainOverlay_QueueRemoveActor(void *);

#endif
