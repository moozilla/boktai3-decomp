#ifndef GUARD_SCENE_INPUT_H
#define GUARD_SCENE_INPUT_H
#include "engine.h"
#include "contact_actor.h"
#include "input.h"

struct SceneInputResource {
    u32 words[8];
};
struct SceneInputPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct SceneInputPackedPosition {
    u32 x : 16, y : 16, z : 16;
};
struct SceneInputWidget {
    u8 unknown0[2];
    u16 stamp; /* Copied from 03005238 on initialization; meaning tentative. */
    u8 unknown4[4];
    u32 flags;
    u16 fieldC, attributeE, scale;
    u8 unknown12[8];
    u8 attribute1A, attribute1B;
    u8 unknown1C[4];
    struct ContactPosition position;
    u8 unknown28[0x12];
    u16 paletteBank;
    u8 unknown3C[0xC];
    u16 *palette;
    u8 unknown4C[0x14];
};
struct SceneInputPair {
    struct SceneInputResource resource;
    struct SceneInputWidget widgets[2];
    struct SceneInputPosition targets[2];
    u16 frame, duration;
};

s32 Anim_SelectWidgetGlyph(void *, void *, u32);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);
void Palette_UpdateHighlightPulse(u16 *, u8 *);
void SceneInput_BeginChoice(struct SceneInputWidget *, void *, u8 *, u32, u8 *);
s32 SceneInput_TickChoice(struct SceneInputWidget *, void *, u8 *, u8 *);
void SceneInput_InitGlyphPair(struct SceneInputPair *);
void SceneInput_DestroyGlyphPair(struct SceneInputPair *);
s32 SceneInput_MapGlyphIndex(s32);
void SceneInput_SetGlyphPair(struct SceneInputPair *, s32, s32);
void SceneInput_GetGridPosition(struct SceneInputPosition *, s32);
void SceneInput_InitMarkerPair(struct SceneInputPair *);
void SceneInput_DestroyMarkerPair(struct SceneInputPair *);
#endif
