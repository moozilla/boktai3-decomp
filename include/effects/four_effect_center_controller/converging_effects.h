#ifndef GUARD_CONVERGING_EFFECTS_H
#define GUARD_CONVERGING_EFFECTS_H
#include "engine/shared/engine.h"
#include "engine/shared/player.h"
#include "engine/shared/monster_particle_random.h"
void ConvergingEffects_UpdateParticles(struct Player *);
void ConvergingEffects_EmitNearParticle(struct Player *, struct SceneVec *);
void ConvergingEffects_EmitFarParticle(struct Player *, struct SceneVec *);
void ConvergingEffects_RemoveSprites(struct Player *);
void ConvergingEffects_InitSprites(struct Player *);
#endif
