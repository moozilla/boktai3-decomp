#ifndef GUARD_SCENE_INPUT_H
#define GUARD_SCENE_INPUT_H
#include "engine.h"
#include "contact_actor.h"

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
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
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

extern u16 gUnk_03005260[];
s32 sub_08219728(void *, void *, u32);
void sub_0821983C(void *, void *, u32, u32, u32, u32, u32, void *);
void sub_0816381C(u16 *, u8 *);
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
