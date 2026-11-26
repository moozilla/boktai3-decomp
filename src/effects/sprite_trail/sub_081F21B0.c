#include "effects/sprite_trail/sprite_trail.h"

void Vector3_CopyComponents(struct TrailVector *destination, struct TrailVector *source)
{
    destination->x = source->x;
    destination->y = source->y;
    destination->z = source->z;
}
