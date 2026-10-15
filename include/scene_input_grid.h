#ifndef GUARD_SCENE_INPUT_GRID_H
#define GUARD_SCENE_INPUT_GRID_H
#include "scene_input.h"
struct SceneInputMovingWidget {
    u32 flags;
    u8 unknown4[0x14];
    struct SceneInputPosition position;
    u8 unknown20[0x40];
};
struct SceneInputGrid {
    u8 kind, row, column;
    u8 unknown3[3];
    u8 fallback, flags, repeat;
    u8 unknown9[3];
    s8 cells[9][4];
};
void sub_0805E484(struct SceneInputPair *, const u32 *, const struct SceneInputPosition *,
                  const struct SceneInputPosition *, const u32 *, const u32 *, u32);
s32 SceneInput_TickPairTransition(struct SceneInputPair *);
s8 SceneInput_GetGridSelection(struct SceneInputGrid *);
void sub_0805E5E4(struct SceneInputGrid *);
void SceneInput_SetGridFlags(struct SceneInputGrid *, u32);
u32 SceneInput_TestGridFlags(struct SceneInputGrid *, u32);
#endif
