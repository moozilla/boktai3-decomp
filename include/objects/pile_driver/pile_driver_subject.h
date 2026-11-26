#ifndef GUARD_SCENE_PANEL_SUBJECT_H
#define GUARD_SCENE_PANEL_SUBJECT_H
#include "objects/pile_driver/pile_driver_commands.h"
#include "scene/collision/contact_swarm.h"

/* Tentative subject view; state-specific field meanings remain under review. */

extern void (*gUnk_08605AF8[])(struct ScenePanelSubject *);
s32 PileDriver_TickLifecycle(struct ScenePanel *);
s32 PileDriver_DestroyLifecycle(struct ScenePanel *);
s32 PileDriver_InitializeLifecycle(struct ScenePanel *);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
struct ScenePanel *PileDriver_Create(u32);
void PileDriverSubject_ResetPosition(struct ScenePanelSubject *);
void PileDriverSubject_KeepPlayerOutside(struct ScenePanelSubject *);
void PileDriverSubject_ApplyContact(struct ContactCollider *, struct ContactCollider *, void *);
void sub_0805CD60(struct ScenePanelSubject *);
#endif
