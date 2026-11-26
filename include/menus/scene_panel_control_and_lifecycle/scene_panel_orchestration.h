#ifndef GUARD_SCENE_PANEL_ORCHESTRATION_H
#define GUARD_SCENE_PANEL_ORCHESTRATION_H
#include "menus/scene_panel_control_and_lifecycle/scene_panel_interaction.h"
#include "system/save_data/script_reference.h"
#include "menus/scene_panel_control_and_lifecycle/scene_panel_config.h"

#define SCENE_PANEL_GLOBAL ((struct ScenePanel *)gScenePanel)
void ScenePanel_InitRig(struct ScenePanel *);
void ScenePanel_InitBackground(struct ScenePanel *);
void ScenePanel_InitOverlay(struct ScenePanel *);
#endif
