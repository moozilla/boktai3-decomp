#ifndef GUARD_SCENE_INPUT_NAVIGATION_H
#define GUARD_SCENE_INPUT_NAVIGATION_H
#include "scene_input.h"
/* Navigation view of the same grid storage used by scene_input_grid.h.
 * Physical axes follow the directional-key dispatcher; cells are column-major. */
struct SceneInputGridView {
 u8 kind,cursorX,cursorY,selection;u8 savedX,savedY;u8 savedSelection,flags,repeat;u8 unknown9[3];
 s8 cells[36];
};
s8 sub_0805E5D0(struct SceneInputGridView *);
u32 sub_0805E7D8(struct SceneInputGridView *);
u32 sub_0805E820(struct SceneInputGridView *);
u32 sub_0805E880(struct SceneInputGridView *);
u32 sub_0805E8E0(struct SceneInputGridView *);
u32 sub_0805E940(struct SceneInputGridView *);
void sub_0805E7C4(struct SceneInputGridView *,u32);
#endif
