#ifndef GUARD_SCENE_PANEL_SUBJECT_TAIL_H
#define GUARD_SCENE_PANEL_SUBJECT_TAIL_H
#include "scene_panel_subject.h"

void sub_08231568(void *);
void sub_082314C4(void *);
u32 sub_0823233C(void *);
void sub_0821FF7C(void *, u32, u32, u32);
void ScenePanelSubject_UpdateTimedEffect(struct ScenePanelSubjectView *);
void ScenePanelSubject_UpdatePaletteBank(struct ScenePanelSubjectView *);
s32 ScenePanelSubject_Update(struct ScenePanelSubjectView *);
s32 ScenePanelSubject_Destroy(struct ScenePanelSubjectView *);
void ScenePanelSubject_InitCollider(struct ScenePanelSubjectView *, u32);
s32 ScenePanelSubject_Initialize(struct ScenePanelSubjectView *, const struct ContactPosition *,
                                 u32, u32, u32, u32, u32, u32, u32);
#endif
