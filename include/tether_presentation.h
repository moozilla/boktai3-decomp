#ifndef GUARD_TETHER_PRESENTATION_H
#define GUARD_TETHER_PRESENTATION_H
#include "engine.h"
#include "projected_display.h"
#include "actor_graphics_and_basic_states.h"
struct TetherIndicatorResource {
    u32 words[8];
};
struct TetherRequestIndicator {
    struct Task task;
    struct ProjectedDisplayEntry animation;
    struct TetherIndicatorResource resource;
    struct ContactPosition projected;
    s16 animationIndex, visible;
    s32 request;
    struct StateActor *actor;
};
struct ScenePaletteFade {
    struct Task task;
    void *background;
    u8 *firstPalette, *secondPalette;
    u32 frame, blendShift, delay;
    void (*update)(struct ScenePaletteFade *);
    u32 timer;
};
struct ScreenPaletteResource {
    u8 header[0x14];
    u16 colors[256];
};
struct ContactPosition *TetherActor_GetPosition(void);
u32 TetherActor_GetRequestMode(void);
s32 DirectionMarker_GetAnimation(s32, s32);
s32 DirectionMarker_Update(struct TetherRequestIndicator *);
s32 TetherActor_IsNearRequestTerrain(void *);
void TetherActor_PositionRequestIndicator(struct TetherRequestIndicator *);
void TetherActor_UpdateRequestIndicator(void *);
void TetherActor_DestroyRequestIndicator(void *);
s32 TetherActor_InitRequestIndicator(struct TetherRequestIndicator *);
void *TetherActor_CreateRequestIndicator(void);
void ScenePaletteFade_SetState(struct ScenePaletteFade *, void (*)(struct ScenePaletteFade *));
void ScenePaletteFade_BlendPaletteBanks(u8 *, u8 *, u8 *, s32, s32, s32);
void PaletteScreen_LoadResources(struct ScenePaletteFade *);
void ScenePaletteFade_SetBackgroundOffsets(void *);
void ScenePaletteFade_BeginDelay(struct ScenePaletteFade *);
void ScenePaletteFade_BeginBlend(struct ScenePaletteFade *);
void ScenePaletteFade_BeginHold(struct ScenePaletteFade *);
void ScenePaletteFade_UpdateDelay(struct ScenePaletteFade *);
void ScenePaletteFade_UpdateBlend(struct ScenePaletteFade *);
void ScenePaletteFade_UpdateHold(struct ScenePaletteFade *);
void ScenePaletteFade_Update(void *);
void ScenePaletteFade_Destroy(void *);
s32 ScenePaletteFade_InitFromScript(struct ScenePaletteFade *, void *);
void *ScenePaletteFade_Create(void *);
#endif
