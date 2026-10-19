#ifndef GUARD_SCENE_PANEL_LIFECYCLE_H
#define GUARD_SCENE_PANEL_LIFECYCLE_H
#include "scene_panel_commands.h"

extern void (*const gUnk_08605ABC[])(struct ScenePanel *);
void ScenePanel_ReadInitialMode(struct ScenePanel *);
void ScenePanel_InitAnchorPositions(struct ScenePanel *);
void ScenePanel_CreateSubjects(struct ScenePanel *);
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
