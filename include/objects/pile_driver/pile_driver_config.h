#ifndef GUARD_PILE_DRIVER_CONFIG_H
#define GUARD_PILE_DRIVER_CONFIG_H
#include "objects/pile_driver/pile_driver_rig.h"

struct PileDriverSubject;
struct PileDriverSubject *PileDriverSubject_Create(const struct ContactPosition *, u32, u32, u32,
                                                   u32, u32, u32, u32);
void PileDriver_InitScheduledAction(struct PileDriver *);
void PileDriver_InitPaletteSprite(struct PileDriver *);
void PileDriver_InitAuxiliarySprite(struct PileDriver *);
void PileDriver_InitResourceConfiguration(struct PileDriver *);
s32 PileDriver_AreSubjectsIdle(struct PileDriver *);
#endif
