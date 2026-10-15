#ifndef GUARD_SCENE_PANEL_LIFECYCLE_H
#define GUARD_SCENE_PANEL_LIFECYCLE_H
#include "scene_panel_commands.h"
struct ScenePanelLifecycleView {
    u8 unknown0[0x1A];
    u16 initialMode;
    u8 unknown1C[2];
    u8 subjectCount, activeCount;
    u8 unknown20[0x234];
    struct ScenePanelPaletteSprite paletteSprite;
    u8 unknown2C4[0x230];
    struct ScenePanelInteractionPlayer *savedPlayer;
    struct TetherActor *savedTether;
    u16 state, frame;
    struct ContactPosition origin, approachPosition, center;
    struct ScenePanelAuxSprite auxiliary;
    u8 unknown560[0x6AF];
    u8 skipMainSprite;
    u8 mode;
    u8 unknownC11[0x5B];
    struct ScenePanelRig rig;
    u8 unknown1320[0x22];
    u8 activated, warningStage;
    u16 warningSounds[4];
};
extern void (*const gUnk_08605ABC[])(struct ScenePanel *);
void ScenePanel_ReadInitialMode(struct ScenePanel *);
void ScenePanel_InitAnchorPositions(struct ScenePanelConfigView *);
void ScenePanel_CreateSubjects(struct ScenePanelConfigView *);
void ScenePanel_InitScriptProperty(struct ScenePanel *);
void ScenePanel_UpdateParticles(struct ScenePanel *);
void ScenePanel_DestroyParticles(struct ScenePanel *);
void ScenePanel_InitParticles(struct ScenePanel *);
void ScenePanel_InitTimePalette(struct ScenePanel *);
void ScenePanel_UpdatePaletteState(struct ScenePanel *);
void ScenePanel_DrawBackgroundLayers(struct ScenePanel *);
void ScenePanel_NoOpAction(struct ScenePanel *);
void ScenePanel_InitializeInteraction(struct ScenePanel *);
#endif
