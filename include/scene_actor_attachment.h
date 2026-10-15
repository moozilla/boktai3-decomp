#ifndef GUARD_SCENE_ACTOR_ATTACHMENT_H
#define GUARD_SCENE_ACTOR_ATTACHMENT_H
#include "global.h"
/* The category-specific pairs feed Y/Z offset arguments of attachment creators. */
void sub_0807C11C(u8 event, u8 category, u8 variant);
void sub_0807C374(u8 gate, u8 event, u8 category, u8 variant);
void sub_0807C394(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C3D8(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C424(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C468(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C4B4(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C4F4(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C53C(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C580(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C5C4(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C604(u8 kind, s32 *yOffset, s32 *zOffset);
/* Unhandled category/kind combinations leave the caller's outputs unchanged. */
void sub_0807C644(u8 category, u8 kind, s32 *yOffset, s32 *zOffset);
struct SceneActorEffectCandidate {
    s32 key;
    u32 value;
};
void sub_0807CD80(struct SceneActorEffectCandidate *, s32 count);
#endif
