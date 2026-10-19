#ifndef GUARD_SCENE_PANEL_SUBJECT_H
#define GUARD_SCENE_PANEL_SUBJECT_H
#include "scene_panel_commands.h"
#include "contact_swarm.h"

/* Tentative subject view; state-specific field meanings remain under review. */

extern void (*gUnk_08605AF8[])(struct ScenePanelSubject *);
s32 ScenePanel_TickLifecycle(struct ScenePanel *);
s32 ScenePanel_DestroyLifecycle(struct ScenePanel *);
s32 ScenePanel_InitializeLifecycle(struct ScenePanel *);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
struct ScenePanel *ScenePanel_Create(u32);
void ScenePanelSubject_ResetPosition(struct ScenePanelSubject *);
void ScenePanelSubject_KeepPlayerOutside(struct ScenePanelSubject *);
void ScenePanelSubject_ApplyContact(struct ContactCollider *, struct ContactCollider *, void *);
void sub_0805CD60(struct ScenePanelSubject *);
#endif
