#ifndef GUARD_SCENE_PANEL_H
#define GUARD_SCENE_PANEL_H
#include "engine.h"
#include "background_controller.h"
#include "contact_actor.h"
#include "scene_object.h"
/* Tentative presentation/state view; fields belong to observed consumers. */
struct ScenePanel {
    u8 unknown0[0x22E];
    u16 stateFrame, channels[8];
    u8 unknown240[0x10];
    void (*state)(struct ScenePanel *);
    u8 unknown254[0x280];
    void *backgroundMap, *palette;
    void *overlayMap, *overlayPalette;
    u8 targetLevels[4], currentLevels[4];
    u16 overlayFrame;
    u8 unknown4EE[0x22];
    struct ContactPackedVector center;
    u8 unknown518[0x6F8];
    u8 paletteMode;
    u8 unknownC11[0x717];
    u16 mode;
    s16 shakeOffset;
    u8 unknown132C[0xE];
    u8 resourceIndex;
    u8 unknown133B[0x3D];
    u16 effectPhase;
    u8 unknown137A[0xAE];
    u32 transitionComplete;
    u8 unknown142C[4];
    u16 channelsDirty;
};
extern const u16 gUnk_08605A88[];
extern u16 gUnk_03004BD8;
void ScenePanel_SetChannelValue(struct ScenePanel *, s32, u16);
void ScenePanel_SetState(struct ScenePanel *, void (*)(struct ScenePanel *));
s32 ScenePanel_GetPaletteOffset(struct ScenePanel *);
void sub_08058168(struct ScenePanel *);
void sub_08058184(struct ScenePanel *);
void ScenePanel_BeginForwardSequence(struct ScenePanel *);
void sub_0805822C(struct ScenePanel *);
void ScenePanel_BeginLongSequence(struct ScenePanel *);
void ScenePanel_BeginPulseSequence(struct ScenePanel *);
void ScenePanel_LoadModePalette(struct ScenePanel *);
void ScenePanel_SetPresentationMode(struct ScenePanel *);
void ScenePanel_TickForwardThree(struct ScenePanel *);
void ScenePanel_TickForwardFour(struct ScenePanel *);
void ScenePanel_TickReverseFour(struct ScenePanel *);
void ScenePanel_TickReverseThree(struct ScenePanel *);
void ScenePanel_TickLongSix(struct ScenePanel *);
void ScenePanel_TickLongSeven(struct ScenePanel *);
void ScenePanel_TickLongEight(struct ScenePanel *);
void ScenePanel_TickLongNine(struct ScenePanel *);
void ScenePanel_TickLongTen(struct ScenePanel *);
void ScenePanel_TickLongEleven(struct ScenePanel *);
void ScenePanel_TickPulseRest(struct ScenePanel *);
void ScenePanel_TickPulseTwelve(struct ScenePanel *);
void ScenePanel_TickPulseThirteen(struct ScenePanel *);
void ScenePanel_TickPulseTwo(struct ScenePanel *);
extern struct ScenePanel *gUnk_020004A4;
void ScenePanel_CallPendingScript(struct ScenePanel *, u32);
union ScenePanelPosition;
void ScenePanel_AddParticle(struct ScenePanel *, union ScenePanelPosition *,
                            union ScenePanelPosition *, u32, u32);
void sub_0806A160(const void *, s32, const void *, s32, s32, s32, s32, s32);
#endif
