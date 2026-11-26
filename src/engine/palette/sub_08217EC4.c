#include "engine/shared/sprite_drawable_api.h"

void Effect_SetOrigin(struct SpriteDrawable *object, s32 a, s32 b)
{
    object->offsetX = a;
    object->offsetY = b;
}
