#ifndef GUARD_SCALED_SHADOW_H
#define GUARD_SCALED_SHADOW_H
#include "terrain_shadow.h"

struct ScaledShadowSprite {
    u32 flags;
    u8 queued, enabled, rotation, priority;
    u8 scaleX, scaleY, unknownA[2];
    const void *resource;
    u16 frame;
    u8 frameIndex, elapsed, width, height, originX, originY;
    u32 attributes;
    struct SceneVec position;
    struct ScaledShadowSprite *previous, *next;
};
struct ShadowAnimationResource {
    u8 flags, unknown1;
    u16 unknown2, unknown4, palette;
    u8 unknown8[4];
    void *paletteData, *tileData, *frames;
    u32 unknown18;
};
struct ScaledShadow {
    u8 active, flags;
    u16 unknown2, unknown4, unknown6;
    struct SceneVec *source;
    struct ScaledShadowSprite sprite;
    struct ShadowAnimationResource resource;
    s8 scale;
    u8 minScale, maxScale, unknown57;
    s16 maxHeight, minHeight;
    u16 shift, nearShift;
    void (*update)(struct ScaledShadow *);
    struct ScaledShadow *previous, *next;
};
struct ScaledShadowManager {
    struct Task task;
    u32 unknown18;
    struct ScaledShadow *head;
};
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
