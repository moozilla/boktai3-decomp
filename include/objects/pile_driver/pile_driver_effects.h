#ifndef GUARD_PILE_DRIVER_EFFECTS_H
#define GUARD_PILE_DRIVER_EFFECTS_H
#include "engine/engine.h"
#include "engine/math/scene_vec.h"
#include "engine/video/render.h"
struct PileDriver;
/* Additional view of PileDriver's particle and time-phase palette consumers. */
union PileDriverPosition {
    struct SceneVec world;
    u32 words[2];
};
struct PileDriverParticle {
    u32 flags;
    u8 unknown4[11], renderMode, unknown10[8];
    union PileDriverPosition position;
    u8 unknown20[8];
    union PileDriverPosition velocity;
    u8 active, age, lifetime, unknown33;
};
struct PileDriverParticles {
    void *resource;
    struct PileDriverParticle entries[32];
    u8 active, cursor;
};
struct PileDriverGraphics {
    u8 unknown0[12];
    u16 *paletteOverride;
    u8 unknown10[12];
};
struct PileDriverMainSprite {
    u32 flags;
    u8 unknown4[0x28];
    struct PileDriverGraphics graphics;
    u16 paletteColors[16];
    u16 active, paletteFrame, unknown6C, phaseCounter;
};
struct PileDriverEffectsView {
    u8 unknown0[0x20];
    u16 paletteColors[16][16];
    const u16 *paletteSources[3];
    u8 previousPalette, targetPalette;
    u16 stateFrame, channels[8];
    u8 unknown240[0x10];
    void (*state)(struct PileDriver *);
    struct PileDriverMainSprite mainSprite;
    u8 unknown2C4[0x210];
    void *backgroundMap, *palette;
    u8 unknown4DC[0x20];
    u16 effectState, effectFrame;
    u8 unknown500[0x60];
    void *particleResource;
    struct PileDriverParticle particleEntries[32];
    u8 particlesActive, particleCursor;
    u8 unknownBE6[0x25], timePhase, unknownC0C[4], paletteMode;
    u8 unknownC11[0x717];
    u16 mode;
    u8 unknown132A[0x10], resourceIndex, unknown133B[0xED];
    u32 transitionComplete;
    u16 paletteBlendFrame, paletteBlendActive, channelsDirty;
    u8 unknown1432[0x37A];
    u32 pendingScriptId;
};
void PileDriver_SetEffectState(struct PileDriver *, u16);
void PileDriver_CallPendingScript(struct PileDriver *, u32);
void PileDriver_UpdateParticles(struct PileDriver *);
void PileDriver_AddParticle(struct PileDriver *, union PileDriverPosition *,
                            union PileDriverPosition *, u32, u32);
void PileDriver_DestroyParticles(struct PileDriver *);
void PileDriver_InitParticles(struct PileDriver *);
void PileDriver_SelectTimePalette(struct PileDriver *, s32);
s32 PileDriver_IsSpriteVisible(struct PileDriverMainSprite *);
s32 PileDriver_AdvancePaletteFrame(struct PileDriverMainSprite *);
void PileDriver_InitTimePalette(struct PileDriver *);
void PileDriver_UpdateTimePalette(struct PileDriverEffectsView *);
void ParticleSprite_Initialize(struct PileDriverParticle *, void *, u32);
void SpriteDrawable_SelectTileFrame(struct PileDriverParticle *, void *, u32);
void Effect_SetOrigin(struct PileDriverParticle *, s32, s32);
void Effect_SetPalette(struct PileDriverParticle *, u32);
u32 SunClock_GetPhase(void);

#endif
