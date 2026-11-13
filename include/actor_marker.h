#ifndef GUARD_ACTOR_MARKER_H
#define GUARD_ACTOR_MARKER_H
#include "scene_combat_actor.h"
#include "engine.h"
#include "ram.h"
struct ActorMarkerEffectPosition {
    u32 x : 16, y : 16, z : 16, pad : 16;
};

extern const u16 gUnk_0824E6F8[][4];

extern u8 gUnk_0824E768[];

extern const u16 gUnk_0824E778[][10][4];

extern u8 gUnk_0824E770[];

struct ActorMarkerFragmentOffset {
    s16 x, y, z, pad;
};

extern const struct ActorMarkerFragmentOffset gUnk_0824E818[18];
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void ActorMarker_InitFacingSprite(struct SceneCombatActor *);
void ActorMarker_InitBouncingParticles(struct SceneCombatActor *);
void Sound_PlaySongChecked(u32);
#endif
