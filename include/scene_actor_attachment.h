#ifndef GUARD_SCENE_ACTOR_ATTACHMENT_H
#define GUARD_SCENE_ACTOR_ATTACHMENT_H
#include "global.h"
/* The category-specific pairs feed Y/Z offset arguments of attachment creators. */
void SceneActorEffect_PlayCategorySound(u8 event, u8 category, u8 variant);
void SceneActorEffect_PlayCategorySoundIfEnabled(u8 gate, u32 event, u32 category, u32 variant);
void SceneActorAttachment_SelectCategory4Offsets(u8 kind, s32 *yOffset, s32 *zOffset);
void SceneActorAttachment_SelectCategory1Offsets(u8 kind, s32 *yOffset, s32 *zOffset);
void SceneActorAttachment_SelectCommonOffsets(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C468(u8 kind, s32 *yOffset, s32 *zOffset);
void SceneActorAttachment_SelectCategory3Offsets(u8 kind, s32 *yOffset, s32 *zOffset);
void SceneActorAttachment_SelectCategory25Offsets(u8 kind, s32 *yOffset, s32 *zOffset);
void sub_0807C53C(u8 kind, s32 *yOffset, s32 *zOffset);
void SceneActorAttachment_SelectCategory21Offsets(u8 kind, s32 *yOffset, s32 *zOffset);
void SceneActorAttachment_SelectCategory10Offsets(u8 kind, s32 *yOffset, s32 *zOffset);
void SceneActorAttachment_SelectCategory23Offsets(u8 kind, s32 *yOffset, s32 *zOffset);
/* Unhandled category/kind combinations leave the caller's outputs unchanged. */
void SceneActorAttachment_SelectOffsets(u8 category, u8 kind, s32 *yOffset, s32 *zOffset);
struct SceneActorEffectCandidate {
    s32 key;
    u32 value;
};
void SceneActorEffect_SortCandidates(struct SceneActorEffectCandidate *, s32 count);
#endif
