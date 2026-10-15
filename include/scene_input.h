#ifndef GUARD_SCENE_INPUT_H
#define GUARD_SCENE_INPUT_H
#include "engine.h"

struct SceneInputResource {
    u32 words[8];
};
struct SceneInputWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x3C];
    u16 *palette;
    u8 unknown4C[0x14];
};
struct SceneInputPair {
    struct SceneInputResource resource;
    struct SceneInputWidget widgets[2];
};
struct SceneInputPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct SceneInputPackedPosition {
    u32 x : 16, y : 16, z : 16;
};
extern u16 gUnk_03005260[];
s32 sub_08219728(void *, void *, u32);
void sub_0821983C(void *, void *, u32, u32, u32, u32, u32, void *);
void sub_0816381C(u16 *, u8 *);
void sub_0805E190(struct SceneInputWidget *, void *, u8 *, u32, u8 *);
s32 sub_0805E1D0(struct SceneInputWidget *, void *, u8 *, u8 *);
void sub_0805E270(struct SceneInputPair *);
void sub_0805E2F0(struct SceneInputPair *);
s32 sub_0805E30C(s32);
void sub_0805E31C(struct SceneInputPair *, s32, s32);
void sub_0805E394(struct SceneInputPosition *, s32);
void sub_0805E3FC(struct SceneInputPair *);
void sub_0805E468(struct SceneInputPair *);
#endif
