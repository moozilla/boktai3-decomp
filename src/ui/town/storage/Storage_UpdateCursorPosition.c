#include "global.h"
#include "ui/widgets/scene_input_navigation.h"
#include "engine/video/anim_sprite.h"
void Storage_GetSlotPosition(void *, u32);
void Storage_UpdateCursorPosition(struct SceneInputGridView *grid, struct AnimSprite *cursor)
{
    Storage_GetSlotPosition(&cursor->position, grid->selection);
}
