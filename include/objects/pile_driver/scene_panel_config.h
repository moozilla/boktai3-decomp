#ifndef GUARD_SCENE_PANEL_CONFIG_H
#define GUARD_SCENE_PANEL_CONFIG_H
#include "objects/pile_driver/scene_panel_rig.h"

struct ScenePanelSubject;
struct ScenePanelSubject *ScenePanelSubject_Create(const struct ContactPosition *, u32, u32, u32,
                                                   u32, u32, u32, u32);
void ScenePanel_InitScheduledAction(struct ScenePanel *);
void ScenePanel_InitPaletteSprite(struct ScenePanel *);
void ScenePanel_InitAuxiliarySprite(struct ScenePanel *);
void ScenePanel_InitResourceConfiguration(struct ScenePanel *);
s32 ScenePanel_AreSubjectsIdle(struct ScenePanel *);
#endif
