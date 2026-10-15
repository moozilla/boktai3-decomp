#ifndef GUARD_SCENE_INPUT_NAVIGATION_H
#define GUARD_SCENE_INPUT_NAVIGATION_H
#include "scene_input.h"
/* Navigation view of the same grid storage used by scene_input_grid.h.
 * Physical axes follow the directional-key dispatcher; cells are column-major. */
struct SceneInputGridView {
    u8 kind, cursorX, cursorY, selection;
    u8 savedX, savedY;
    u8 savedSelection, flags, repeat;
    u8 unknown9[3];
    s8 cells[36];
};
s8 SceneInput_GetGridSelection(struct SceneInputGridView *);
u32 SceneInput_GetNavigationKeys(struct SceneInputGridView *);
u32 SceneInput_MoveUp(struct SceneInputGridView *);
u32 SceneInput_MoveDown(struct SceneInputGridView *);
u32 SceneInput_MoveLeft(struct SceneInputGridView *);
u32 SceneInput_MoveRight(struct SceneInputGridView *);
void SceneInput_SetGridFlags(struct SceneInputGridView *, u32);
#endif
