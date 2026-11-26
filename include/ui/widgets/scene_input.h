#ifndef GUARD_SCENE_INPUT_H
#define GUARD_SCENE_INPUT_H
#include "engine/engine.h"
#include "engine/video/palette_pulse.h"
#include "scene/collision/contact_actor.h"
#include "engine/video/anim_sprite.h"
#include "engine/input.h"

struct SceneInputPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct SceneInputPackedPosition {
    u32 x : 16, y : 16, z : 16;
};
struct SceneInputPair {
    struct AnimSpriteResource resource;
    struct AnimSprite widgets[2];
    struct SceneInputPosition targets[2];
    u16 frame, duration;
};

s32 AnimSprite_SelectGlyph(void *, void *, u32);
s32 AnimSprite_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);
void SceneInput_BeginChoice(struct AnimSprite *, void *, u8 *, u32, u8 *);
s32 SceneInput_TickChoice(struct AnimSprite *, void *, u8 *, u8 *);
void SceneInput_InitGlyphPair(struct SceneInputPair *);
void SceneInput_DestroyGlyphPair(struct SceneInputPair *);
s32 SceneInput_MapGlyphIndex(s32);
void SceneInput_SetGlyphPair(struct SceneInputPair *, s32, s32);
void SceneInput_GetGridPosition(struct SceneInputPosition *, s32);
void SceneInput_InitMarkerPair(struct SceneInputPair *);
void SceneInput_DestroyMarkerPair(struct SceneInputPair *);
#endif
