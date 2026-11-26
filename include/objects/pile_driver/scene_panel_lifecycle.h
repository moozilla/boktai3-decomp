#ifndef GUARD_SCENE_PANEL_LIFECYCLE_H
#define GUARD_SCENE_PANEL_LIFECYCLE_H
#include "objects/pile_driver/pile_driver_commands.h"

extern void (*const gUnk_08605ABC[])(struct ScenePanel *);
void PileDriver_ReadInitialMode(struct ScenePanel *);
void PileDriver_InitAnchorPositions(struct ScenePanel *);
void PileDriver_CreateSubjects(struct ScenePanel *);
void PileDriver_InitScriptProperty(struct ScenePanel *);
void PileDriver_UpdateParticles(struct ScenePanel *);
void PileDriver_DestroyParticles(struct ScenePanel *);
void PileDriver_InitParticles(struct ScenePanel *);
void PileDriver_InitTimePalette(struct ScenePanel *);
void PileDriver_UpdatePaletteState(struct ScenePanel *);
void PileDriver_DrawBackgroundLayers(struct ScenePanel *);
void PileDriver_NoOpAction(struct ScenePanel *);
void PileDriver_InitializeInteraction(struct ScenePanel *);
#endif
