#ifndef GUARD_PILE_DRIVER_SUBJECT_H
#define GUARD_PILE_DRIVER_SUBJECT_H
#include "objects/pile_driver/pile_driver_commands.h"
#include "scene/collision/contact_swarm.h"

/* Tentative subject view; state-specific field meanings remain under review. */

extern void (*gUnk_08605AF8[])(struct PileDriverSubject *);
s32 PileDriver_TickLifecycle(struct PileDriver *);
s32 PileDriver_DestroyLifecycle(struct PileDriver *);
s32 PileDriver_InitializeLifecycle(struct PileDriver *);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
struct PileDriver *PileDriver_Create(u32);
void PileDriverSubject_ResetPosition(struct PileDriverSubject *);
void PileDriverSubject_KeepPlayerOutside(struct PileDriverSubject *);
void PileDriverSubject_ApplyContact(struct ContactCollider *, struct ContactCollider *, void *);
void PileDriverSubject_TickInactive(struct PileDriverSubject *);
#endif
