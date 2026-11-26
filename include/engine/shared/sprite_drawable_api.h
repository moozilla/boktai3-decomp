#ifndef GUARD_SPRITE_DRAWABLE_API_H
#define GUARD_SPRITE_DRAWABLE_API_H

#include "engine/sprite_oam/sprite_oam.h"

struct SpriteEffectTileResource;

s32 Effect_LinkToList(struct SpriteDrawable *, u32);
void Effect_UnlinkFromList(struct SpriteDrawable *, u32);
void ParticleSprite_Initialize(struct SpriteDrawable *, const struct SpriteEffectTileResource *, u32);
void sub_08217E40(struct SpriteDrawable *, u32);
void Effect_SetOrigin(struct SpriteDrawable *, s32, s32);
void Effect_SetPalette(struct SpriteDrawable *, u32);
void SpriteDrawable_SelectTileFrame(struct SpriteDrawable *,
                                    const struct SpriteEffectTileResource *, u32);

#endif
