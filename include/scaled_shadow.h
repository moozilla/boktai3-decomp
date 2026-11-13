#ifndef GUARD_SCALED_SHADOW_H
#define GUARD_SCALED_SHADOW_H
#include "scaled_shadow_types.h"

s32 ScaledShadowManager_AddShadow(struct ScaledShadowManager *, struct ScaledShadow *);
s32 ScaledShadowManager_RemoveShadow(struct ScaledShadowManager *, struct ScaledShadow *);
void ScaledShadow_Enable(struct ScaledShadow *);
void ScaledShadow_Disable(struct ScaledShadow *);
void ScaledShadow_UpdateNoOp(struct ScaledShadow *);
void ScaledShadow_Update(struct ScaledShadow *);
u32 ScaledShadowManager_UpdateAll(struct ScaledShadowManager *);
void ScaledShadowManager_OnTaskDestroy(void);
s32 ScaledShadowManager_Init(struct ScaledShadowManager *, void *);
struct ScaledShadowManager *ScaledShadowManager_Create(void *, s32);
s32 ScaledShadow_Initialize(struct ScaledShadow *, struct SceneVec *, s32, s32, s32, s32, s32, s32,
                            s32, s32, s32);
u32 ScaledShadow_InitializeDefault(struct ScaledShadow *, struct SceneVec *);
u32 ScaledShadow_InitializeDefaultSmall(struct ScaledShadow *, struct SceneVec *);
u32 ScaledShadow_InitializeDefaultLarge(struct ScaledShadow *, struct SceneVec *);
u32 ScaledShadow_InitializeRange(struct ScaledShadow *, struct SceneVec *, s32, s32);
u32 ScaledShadow_InitializeSmallRange(struct ScaledShadow *, struct SceneVec *, s32, s32);
u32 ScaledShadow_InitializeLargeRange(struct ScaledShadow *, struct SceneVec *, s32, s32);
u32 ScaledShadow_Destroy(struct ScaledShadow *);
u32 ScaledShadow_SetGraphic(struct ScaledShadow *, u32, u32);
u32 ScaledShadow_SetFlag(struct ScaledShadow *, u32);
void ScaledShadow_SetScaleRange(struct ScaledShadow *, s32, s32, s32, s32, s32, s32, s32, s32);
void Sprite_ApplyAnimation(void *, void *);
#endif
