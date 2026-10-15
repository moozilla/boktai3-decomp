#ifndef GUARD_SCENE_PANEL_H
#define GUARD_SCENE_PANEL_H
#include "engine.h"
#include "background_controller.h"
/* Tentative presentation/state view; fields belong to observed consumers. */
struct ScenePanel {
    u8 unknown0[0x22E];
    u16 stateFrame, channels[8];
    u8 unknown240[0x10];
    void (*state)(struct ScenePanel *);
    u8 unknown254[0x280];
    void *backgroundMap, *palette;
    u8 unknown4DC[0x734];
    u8 paletteMode;
    u8 unknownC11[0x717];
    u16 mode;
    u8 unknown132A[0x10];
    u8 resourceIndex;
    u8 unknown133B[0xED];
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
#endif
