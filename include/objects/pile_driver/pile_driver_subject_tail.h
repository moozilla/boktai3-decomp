#ifndef GUARD_PILE_DRIVER_SUBJECT_TAIL_H
#define GUARD_PILE_DRIVER_SUBJECT_TAIL_H
#include "objects/pile_driver/pile_driver_subject.h"

void EffectGroup_Reset(void *);
void EffectGroup_FadeFirstActive(void *);
u32 SceneActorEffects_DestroyGroup(void *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void PileDriverSubject_UpdateTimedEffect(struct PileDriverSubject *);
void PileDriverSubject_UpdatePaletteBank(struct PileDriverSubject *);
s32 PileDriverSubject_Update(struct PileDriverSubject *);
s32 PileDriverSubject_Destroy(struct PileDriverSubject *);
void PileDriverSubject_InitCollider(struct PileDriverSubject *, u32);
s32 PileDriverSubject_Initialize(struct PileDriverSubject *, const struct ContactPosition *, u32,
                                 u32, u32, u32, u32, u32, u32);
#endif
