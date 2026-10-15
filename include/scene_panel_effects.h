#ifndef GUARD_SCENE_PANEL_EFFECTS_H
#define GUARD_SCENE_PANEL_EFFECTS_H
#include "engine.h"
#include "scene_vec.h"
struct ScenePanel;
/* Additional view of ScenePanel's particle and time-phase palette consumers. */
union ScenePanelPosition {
    struct SceneVec world;
    u32 words[2];
};
struct ScenePanelParticle {
    u32 flags;
    u8 unknown4[11], renderMode, unknown10[8];
    union ScenePanelPosition position;
    u8 unknown20[8];
    union ScenePanelPosition velocity;
    u8 active, age, lifetime, unknown33;
};
struct ScenePanelParticles {
    void *resource;
    struct ScenePanelParticle entries[32];
    u8 active, cursor;
};
struct ScenePanelGraphics {
    u8 unknown0[12];
    u16 *paletteOverride;
    u8 unknown10[12];
};
struct ScenePanelMainSprite {
    u32 flags;
    u8 unknown4[0x28];
    struct ScenePanelGraphics graphics;
    u16 paletteColors[16];
    u16 active, paletteFrame, unknown6C, phaseCounter;
};
struct ScenePanelEffectsView {
    u8 unknown0[0x20];
    u16 paletteColors[16][16];
    const u16 *paletteSources[3];
    u8 previousPalette, targetPalette;
    u16 stateFrame, channels[8];
    u8 unknown240[0x10];
    void (*state)(struct ScenePanel *);
    struct ScenePanelMainSprite mainSprite;
    u8 unknown2C4[0x210];
    void *backgroundMap, *palette;
    u8 unknown4DC[0x20];
    u16 effectState, effectFrame;
    u8 unknown500[0x60];
    void *particleResource;
    struct ScenePanelParticle particleEntries[32];
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
void sub_08057960(struct ScenePanel *, u16);
void sub_08057978(struct ScenePanel *, u32);
void sub_080579B0(struct ScenePanel *);
void sub_08057A44(struct ScenePanel *, union ScenePanelPosition *, union ScenePanelPosition *, u32,
                  u32);
void sub_08057ADC(struct ScenePanel *);
void sub_08057AFC(struct ScenePanel *);
void sub_08057B5C(struct ScenePanel *, s32);
s32 sub_08057C58(struct ScenePanelMainSprite *);
s32 sub_08057C78(struct ScenePanelMainSprite *);
void sub_08057CB8(struct ScenePanel *);
void sub_08057E04(struct ScenePanel *);
void sub_08217DE0(struct ScenePanelParticle *, void *, u32);
void sub_08217EEC(struct ScenePanelParticle *, void *, u32);
void sub_08217EC4(struct ScenePanelParticle *, s32, s32);
void sub_08217ECC(struct ScenePanelParticle *, u32);
void sub_08215284(void *, u32);
u8 *sub_0821A520(u16, u16);
u32 sub_08228DB8(void);
extern u16 gUnk_03004FC0[];
#endif
