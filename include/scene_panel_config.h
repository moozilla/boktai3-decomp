#ifndef GUARD_SCENE_PANEL_CONFIG_H
#define GUARD_SCENE_PANEL_CONFIG_H
#include "scene_panel_rig.h"
struct ScenePanelAuxSprite {
    u32 flags;
    u8 unknown4[3];
    u8 renderMode, parameter8, parameter9;
    u8 unknownA[6];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
    u8 graphics[0x1C];
};
struct ScenePanelPaletteGraphics {
    u8 unknown0[0xC];
    u16 *palette;
    u8 unknown10[0xC];
};
struct ScenePanelPaletteSprite {
    u32 flags;
    u8 unknown4[3];
    u8 renderMode;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
    struct ScenePanelPaletteGraphics graphics;
    u16 paletteCopy[16];
    u16 active, transform[3];
};
struct ScenePanelConfigView {
    u8 unknown0[0x1A];
    u16 category;
    u8 unknown1C[2];
    u8 subjectCount;
    u8 unknown1F[0x235];
    struct ScenePanelPaletteSprite paletteSprite;
    u8 unknown2C4[0x23C];
    struct ContactPosition origin, approachPosition, center;
    struct ScenePanelAuxSprite auxiliary;
    u8 unknown560[0x688];
    struct SceneRigSubject *subjects[4];
    u8 unknownBF8[0x17];
    u8 suppressAuxiliary;
    u8 unknownC10[0x72A];
    u8 resourceIndex;
    u8 unknown133B[8];
    u8 warningStage;
    u16 warningSounds[4];
    u8 unknown134C[6];
    u16 resourceId;
    u8 unknown1354[0x70];
    u16 parameter;
    u8 unknown13C6[0x3E6];
    s32 property;
};
struct ScenePanelSpawnPosition {
    u32 x : 16, y : 16, z : 16;
};
void *sub_0805DCAC(const void *, u32, u32, u32, u32, s32, s32, u32);
void ScenePanel_InitScheduledAction(struct ScenePanel *);
void ScenePanel_InitPaletteSprite(struct ScenePanel *);
void ScenePanel_InitAuxiliarySprite(struct ScenePanel *);
void ScenePanel_InitResourceConfiguration(struct ScenePanelConfigView *);
s32 ScenePanel_AreSubjectsIdle(struct ScenePanel *);
#endif
