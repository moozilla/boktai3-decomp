#ifndef GUARD_SCENE_PANEL_SUBJECT_TAIL_H
#define GUARD_SCENE_PANEL_SUBJECT_TAIL_H
#include "scene_panel_subject.h"

void EffectGroup_Reset(void *);
void sub_082314C4(void *);
u32 SceneActorEffects_DestroyGroup(void *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void ScenePanelSubject_UpdateTimedEffect(struct ScenePanelSubject *);
void ScenePanelSubject_UpdatePaletteBank(struct ScenePanelSubject *);
s32 ScenePanelSubject_Update(struct ScenePanelSubject *);
s32 ScenePanelSubject_Destroy(struct ScenePanelSubject *);
void ScenePanelSubject_InitCollider(struct ScenePanelSubject *, u32);
s32 ScenePanelSubject_Initialize(struct ScenePanelSubject *, const struct ContactPosition *, u32,
                                 u32, u32, u32, u32, u32, u32);
#endif
