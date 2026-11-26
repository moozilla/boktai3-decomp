#ifndef GUARD_SCENE_PANEL_CONFIG_H
#define GUARD_SCENE_PANEL_CONFIG_H
#include "objects/pile_driver/pile_driver_rig.h"

struct ScenePanelSubject;
struct ScenePanelSubject *PileDriverSubject_Create(const struct ContactPosition *, u32, u32, u32,
                                                   u32, u32, u32, u32);
void PileDriver_InitScheduledAction(struct ScenePanel *);
void PileDriver_InitPaletteSprite(struct ScenePanel *);
void PileDriver_InitAuxiliarySprite(struct ScenePanel *);
void PileDriver_InitResourceConfiguration(struct ScenePanel *);
s32 PileDriver_AreSubjectsIdle(struct ScenePanel *);
#endif
