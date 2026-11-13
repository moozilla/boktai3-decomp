#ifndef GUARD_SCENE_ACTOR_LIFECYCLE_H
#define GUARD_SCENE_ACTOR_LIFECYCLE_H

#include "engine.h"
#include "player.h"
#include "scene_actor_components.h"
#include "scene_body_api.h"
#include "display_animation_api.h"
#include "scene_actor_route.h"

struct SceneActorResourceEntry {
    u16 type;
    u8 variant, sound;
    union {
        void *data;
        s8 footstepFrames[4];
    } payload;
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
struct SceneActorPositionLink {
    u16 id, unknown2;
    struct ContactPosition *position;
};
struct SceneActorParameters {
    u32 values[4];
};
struct SceneActorNavigationProbe {
    u8 unknown0[0xC];
    struct ContactPosition current;
    u8 unknown14[8];
    struct ContactPosition target;
    u8 unknown24[0x20];
};
union SceneActorInteraction {
    s8 cues[4];
    struct {
        u8 firstDirection, secondDirection, turnRate, turnState;
    } turning;
};
struct SceneActorPacket {
    u16 id;
    u8 kind, zero;
    u16 reserved;
    u8 command, count;
    u16 args[8];
    u8 unknown18[0x28];
};
struct SceneActor {
    u8 active, variant, stage, phase, state, action, unknown6, finished;
    u8 commandEntered, animationEntered, flashing, enabled;
    u8 unknownC[2];
    u16 flags;
    u8 unknown10[4];
    u32 ticks;
    u8 animationPhase, facing4, facing8, direction;
    u16 defaultPaletteBank, paletteBank;
    u8 unknown20[0x20];
    u32 script;
    struct SceneActorParameters parameters;
    struct ContactPosition target;
    struct SceneActorBody body;
    struct SceneActorPositionLink positionLink;
    u8 unknownAC[0x28];
    union SceneActorDisplay drawable;
    u8 messageReceiver[0x34];
    struct SceneActorRouteCursor route;
    struct SceneActorNavigationProbe probe;
    u8 modeA, modeB;
    u16 modeValue;
    u8 routeProgress, hasDestination, motionRegistered;
    s8 routeId;
    s16 radiusA, radiusB;
    int radiusA32, radiusB32, radiusASquared, radiusBSquared;
    int distance, distanceSquared;
    u16 routeMode, nodeId, routePoint;
    u8 soundEnabled, routeStepFinished, shadowKind, savedAngle;
    u8 registered, controlled;
    int rateA, rateB;
    u8 probeReady, savedAnimationAngle, nodeFacing, savedState;
    int rateC;
    u8 unknown22C[4];
    u8 hasParameters, paramA, paramB, paramC, paramD, unknown235;
    u16 cooldown;
    union SceneActorInteraction interaction;
    struct ContactPosition destination;
    struct SceneActorShadow shadow;
    u8 unknown2B0[8];
    const struct SceneActorAnimationEntry *animationEntries;
    const struct SceneActorResourceEntry *resourceEntry;
    void *attachment;
    u32 effectKind;
    s16 timer, duration;
    u8 unknown2CC[4];
    void (*callbacks[4])(void *, struct SceneActor *);
    void (*destroy)(void *, struct SceneActor *);
    int (*query)(struct SceneActor *);
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
    struct SceneActorPacket packets[2][8];
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
