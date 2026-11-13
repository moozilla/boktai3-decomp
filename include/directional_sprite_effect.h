#ifndef GUARD_DIRECTIONAL_SPRITE_EFFECT_H
#define GUARD_DIRECTIONAL_SPRITE_EFFECT_H
#include "engine.h"
#include "player.h"
#include "display_sequence.h"

void DirectionalSpriteEffect_Activate(struct Player *);
void DirectionalSpriteEffect_LaunchFromFacing(struct Player *, u32,
                                              union DirectionalSpritePosition *);
void DirectionalSpriteEffect_PlaceOppositeAngle(struct Player *, s32,
                                                union DirectionalSpritePosition *);
void DirectionalSpriteEffect_RemoveSprite(struct Player *);
void DirectionalSpriteEffect_InitSprite(struct Player *);
#endif
